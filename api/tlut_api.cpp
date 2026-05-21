#include "tlut_api.hpp"
#include <fstream>
#include <cmath>
#include "xrt/xrt_bo.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"

struct uint128_raw { uint32_t data[4]; };

struct __attribute__((packed)) tlut_config_t {
    int16_t c_sym;
    int16_t upper_threshold;
    int16_t lower_threshold;
    int16_t c_upper;
    int16_t c_lower;
    uint8_t reload_tlut;
    uint8_t use_sym;
    uint8_t use_lin;
    uint8_t padding_1[3];
    uint32_t num_samples;
    uint32_t active_depth; 
};

TlutAccelerator::TlutAccelerator(const std::string& xclbin_path, 
                                 int q_int, 
                                 int q_frac, 
                                 const TlutHardwareConfig& hw_config,
                                 int device_id)
    : q_int_(q_int), q_frac_(q_frac), hw_cfg_(hw_config) {
    
    scale_ = std::pow(2.0, q_frac_);
    format_folder_ = "Q" + std::to_string(q_int_) + "_" + std::to_string(q_frac_);

    auto device = new xrt::device(device_id);
    auto uuid = device->load_xclbin(xclbin_path);
    auto kernel = new xrt::kernel(*device, uuid, "tlut_top");

    // Dynamic memory allocation based on constructor parameters
    bo_in_  = new xrt::bo(*device, hw_cfg_.max_samples * sizeof(int16_t), kernel->group_id(0));
    bo_out_ = new xrt::bo(*device, hw_cfg_.max_samples * sizeof(int16_t), kernel->group_id(1));
    bo_d_   = new xrt::bo(*device, hw_cfg_.dlut_words * sizeof(uint128_raw), kernel->group_id(2));
    bo_e_   = new xrt::bo(*device, hw_cfg_.elut_words * sizeof(uint128_raw), kernel->group_id(3));

    device_ = device; 
    kernel_ = kernel;
}

TlutAccelerator::~TlutAccelerator() {
    delete static_cast<xrt::bo*>(bo_in_); 
    delete static_cast<xrt::bo*>(bo_out_);
    delete static_cast<xrt::bo*>(bo_d_);  
    delete static_cast<xrt::bo*>(bo_e_);
    delete static_cast<xrt::kernel*>(kernel_); 
    delete static_cast<xrt::device*>(device_);
}

double TlutAccelerator::get_fpga_frequency_mhz() const {
    return hw_cfg_.fpga_freq_mhz;
}

void TlutAccelerator::read_txt_to_vector(const std::string& filepath, std::vector<int>& vec) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("[TLUT_API] File not found: " + filepath);
    }
    int val; 
    while (file >> val) vec.push_back(val);
}

void TlutAccelerator::load(const std::string& func_name) {
    std::string base_path = "tluts/" + format_folder_ + "/" + func_name + "/";
    std::vector<int> dlut_raw, elut_raw, ctrl_raw;

    read_txt_to_vector(base_path + "dlut.txt", dlut_raw);
    read_txt_to_vector(base_path + "elut.txt", elut_raw);
    read_txt_to_vector(base_path + "control.txt", ctrl_raw);

    lower_th_ = ctrl_raw[0]; upper_th_ = ctrl_raw[1];
    c_lower_  = ctrl_raw[2]; c_upper_  = ctrl_raw[3];
    c_sym_    = ctrl_raw[4]; use_sym_  = ctrl_raw[5]; use_lin_ = ctrl_raw[6];
    
    active_depth_ = (upper_th_ - lower_th_) + 1;

    // Safety checks for Hardware BRAM limits
    int e_cap = (active_depth_ + 31) / 32;
    int d_cap = (active_depth_ + 16 - 1) / 16;
    int d_words_needed = (d_cap + 7) / 8;

    if (e_cap > hw_cfg_.elut_words || d_words_needed > hw_cfg_.dlut_words) {
        throw std::runtime_error("[TLUT_API] Hardware BRAM overflow. Function requires more LUT memory than allocated.");
    }

    auto& bo_d = *static_cast<xrt::bo*>(bo_d_);
    auto& bo_e = *static_cast<xrt::bo*>(bo_e_);
    uint128_raw* d_map = bo_d.map<uint128_raw*>();
    uint128_raw* e_map = bo_e.map<uint128_raw*>();

    // Clear previous data
    for(size_t i = 0; i < hw_cfg_.dlut_words; i++) d_map[i] = {0,0,0,0};
    for(size_t i = 0; i < hw_cfg_.elut_words; i++) e_map[i] = {0,0,0,0};

    // Pack ELUT (Assumes W_E = 4 bits)
    for (int c = 0; c < e_cap; c++) {
        uint128_raw word = {0, 0, 0, 0};
        for (int j = 0; j < 32; j++) {
            int idx = c * 32 + j;
            if (idx < active_depth_) {
                uint32_t val = elut_raw[idx] & 0xF; 
                word.data[j / 8] |= (val << ((j * 4) % 32));
            }
        }
        e_map[c] = word;
    }

    // Pack DLUT (Assumes B_SIZE = 16)
    for (int c = 0; c < d_words_needed; c++) {
        uint128_raw word = {0, 0, 0, 0};
        for (int j = 0; j < 8; j++) {
            int idx = c * 8 + j;
            if (idx < d_cap) {
                uint32_t val = (uint16_t)dlut_raw[idx]; 
                word.data[j / 2] |= (val << ((j * 16) % 32));
            }
        }
        d_map[c] = word;
    }

    bo_d.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    bo_e.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    tlut_config_t cfg_load = {0};
    cfg_load.reload_tlut = 1;
    cfg_load.active_depth = active_depth_;

    auto& kernel = *static_cast<xrt::kernel*>(kernel_);
    kernel(*static_cast<xrt::bo*>(bo_in_), *static_cast<xrt::bo*>(bo_out_), bo_d, bo_e, cfg_load).wait();
}

std::vector<float> TlutAccelerator::process(const std::vector<float>& input_data) {
    size_t samples_count = input_data.size();

    // Safety check for vector size
    if (samples_count > hw_cfg_.max_samples) {
        throw std::runtime_error("[TLUT_API] Input data exceeds maximum allowed hardware samples.");
    }

    auto& bo_in = *static_cast<xrt::bo*>(bo_in_);
    auto& bo_out = *static_cast<xrt::bo*>(bo_out_);
    auto& kernel = *static_cast<xrt::kernel*>(kernel_);
    
    int16_t* in_map = bo_in.map<int16_t*>();
    int16_t* out_map = bo_out.map<int16_t*>();

    // Quantization (Software Float -> Hardware Q format)
    for (size_t i = 0; i < samples_count; ++i) {
        in_map[i] = static_cast<int16_t>(input_data[i] * scale_);
    }
    bo_in.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    tlut_config_t cfg_run = {0};
    cfg_run.c_sym = c_sym_; 
    cfg_run.upper_threshold = upper_th_; 
    cfg_run.lower_threshold = lower_th_;
    cfg_run.c_upper = c_upper_; 
    cfg_run.c_lower = c_lower_;
    cfg_run.use_sym = use_sym_; 
    cfg_run.use_lin = use_lin_;
    cfg_run.num_samples = samples_count;

    // Execution
    kernel(bo_in, bo_out, *static_cast<xrt::bo*>(bo_d_), *static_cast<xrt::bo*>(bo_e_), cfg_run).wait();
    bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    // De-quantization (Hardware Q format -> Software Float)
    std::vector<float> output_data(samples_count);
    for (size_t i = 0; i < samples_count; ++i) {
        output_data[i] = static_cast<float>(out_map[i]) / scale_;
    }

    return output_data;
}