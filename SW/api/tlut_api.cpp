// tlut_api.cpp
/*
 * ----------------------------------------------------------------------------
 * Implementación de la API TLUT.
 * ----------------------------------------------------------------------------
 */

#include "tlut_api.hpp"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "xrt/xrt_bo.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"

namespace {

struct uint128_raw {
    std::uint32_t data[4];
};

#pragma pack(push, 1)
struct tlut_config_t {
    std::int16_t c_sym;
    std::int16_t upper_threshold;
    std::int16_t lower_threshold;
    std::int16_t c_upper;
    std::int16_t c_lower;
    std::uint8_t reload_tlut;
    std::uint8_t use_sym;
    std::uint8_t use_lin;
    std::uint8_t padding_1[3];
    std::uint32_t num_samples;
    std::uint32_t active_depth;
};
#pragma pack(pop)

std::string join_path(const std::string& lhs, const std::string& rhs) {
    if (lhs.empty()) return rhs;
    if (rhs.empty()) return lhs;
    if (lhs.back() == '/') return lhs + rhs;
    return lhs + "/" + rhs;
}

std::vector<int> read_txt_to_vector(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("[TLUT_API] File not found: " + filepath);
    }

    std::vector<int> vec;
    int val = 0;
    while (file >> val) {
        vec.push_back(val);
    }
    return vec;
}

template <typename T>
constexpr T ceil_div(T value, T divisor) {
    return (value + divisor - 1) / divisor;
}

uint128_raw make_empty_word() {
    return uint128_raw{{0, 0, 0, 0}};
}

uint128_raw pack_elut_word(const std::vector<int>& raw, std::size_t start_idx, std::size_t count) {
    uint128_raw word = make_empty_word();

    for (std::size_t j = 0; j < count; ++j) {
        const std::size_t idx = start_idx + j;
        const std::uint32_t nibble = static_cast<std::uint32_t>(raw[idx]) & 0xFu;
        const std::size_t lane = j / 8u;
        const std::size_t shift = (j % 8u) * 4u;
        word.data[lane] |= (nibble << shift);
    }

    return word;
}

uint128_raw pack_dlut_word(const std::vector<int>& raw, std::size_t start_idx, std::size_t count) {
    uint128_raw word = make_empty_word();

    for (std::size_t j = 0; j < count; ++j) {
        const std::size_t idx = start_idx + j;
        const std::uint32_t val = static_cast<std::uint16_t>(raw[idx]);
        const std::size_t lane = j / 2u;
        const std::size_t shift = (j % 2u) * 16u;
        word.data[lane] |= (val << shift);
    }

    return word;
}

} // namespace

TlutAccelerator::TlutAccelerator(const std::string& xclbin_path,
                                 const TlutHardwareConfig& hw_config,
                                 int device_id)
    : hw_cfg_(hw_config) {
    if (hw_cfg_.q_int <= 0 || hw_cfg_.q_frac < 0) {
        throw std::runtime_error("[TLUT_API] Invalid Q format configuration.");
    }

    format_folder_ = "Q" + std::to_string(hw_cfg_.q_int) + "_" + std::to_string(hw_cfg_.q_frac);

    auto device = std::make_unique<xrt::device>(device_id);
    const auto uuid = device->load_xclbin(xclbin_path);
    auto kernel = std::make_unique<xrt::kernel>(*device, uuid, hw_cfg_.kernel_name.c_str());

    auto bo_in = std::make_unique<xrt::bo>(*device,
                                           hw_cfg_.max_samples * sizeof(sample_t),
                                           kernel->group_id(0));
    auto bo_out = std::make_unique<xrt::bo>(*device,
                                            hw_cfg_.max_samples * sizeof(sample_t),
                                            kernel->group_id(1));
    auto bo_d = std::make_unique<xrt::bo>(*device,
                                           hw_cfg_.dlut_words * sizeof(uint128_raw),
                                           kernel->group_id(2));
    auto bo_e = std::make_unique<xrt::bo>(*device,
                                           hw_cfg_.elut_words * sizeof(uint128_raw),
                                           kernel->group_id(3));

    auto run_load = std::make_unique<xrt::run>(*kernel);
    auto run_process = std::make_unique<xrt::run>(*kernel);

    run_load->set_arg(0, *bo_in);
    run_load->set_arg(1, *bo_out);
    run_load->set_arg(2, *bo_d);
    run_load->set_arg(3, *bo_e);

    run_process->set_arg(0, *bo_in);
    run_process->set_arg(1, *bo_out);
    run_process->set_arg(2, *bo_d);
    run_process->set_arg(3, *bo_e);

    device_ = device.release();
    kernel_ = kernel.release();
    bo_in_ = bo_in.release();
    bo_out_ = bo_out.release();
    bo_d_ = bo_d.release();
    bo_e_ = bo_e.release();
    run_load_ = run_load.release();
    run_process_ = run_process.release();
}

TlutAccelerator::~TlutAccelerator() {
    delete run_load_;
    delete run_process_;
    delete bo_in_;
    delete bo_out_;
    delete bo_d_;
    delete bo_e_;
    delete kernel_;
    delete device_;
}

double TlutAccelerator::get_fpga_frequency_mhz() const noexcept {
    return hw_cfg_.fpga_freq_mhz;
}

double TlutAccelerator::get_last_load_duration_ns() const noexcept {
    return last_load_ns_;
}

double TlutAccelerator::get_last_compute_duration_ns() const noexcept {
    return last_compute_ns_;
}

void TlutAccelerator::execute_run(xrt::run& run_obj, double& telemetry_ns) {
    if (hw_cfg_.enable_profiling) {
        const auto start = std::chrono::steady_clock::now();
        run_obj.start();
        run_obj.wait();
        const auto end = std::chrono::steady_clock::now();
        telemetry_ns = std::chrono::duration<double, std::nano>(end - start).count();
    } else {
        telemetry_ns = 0.0;
        run_obj.start();
        run_obj.wait();
    }
}

void TlutAccelerator::load(const std::string& func_name) {
    const std::string base_path =
        join_path(join_path(hw_cfg_.tluts_dir, format_folder_), func_name);

    const std::vector<int> dlut_raw = read_txt_to_vector(join_path(base_path, "dlut.txt"));
    const std::vector<int> elut_raw = read_txt_to_vector(join_path(base_path, "elut.txt"));
    const std::vector<int> ctrl_raw = read_txt_to_vector(join_path(base_path, "control.txt"));

    if (ctrl_raw.size() < 9) {
        throw std::runtime_error("[TLUT_API] control.txt invalido. Se esperan 9 parametros.");
    }

    lower_th_ = static_cast<std::int16_t>(ctrl_raw[0]);
    upper_th_ = static_cast<std::int16_t>(ctrl_raw[1]);
    c_lower_  = static_cast<std::int16_t>(ctrl_raw[2]);
    c_upper_  = static_cast<std::int16_t>(ctrl_raw[3]);
    c_sym_    = static_cast<std::int16_t>(ctrl_raw[4]);
    use_sym_  = static_cast<std::uint8_t>(ctrl_raw[5]);
    use_lin_  = static_cast<std::uint8_t>(ctrl_raw[6]);

    const std::size_t dlut_lines = static_cast<std::size_t>(ctrl_raw[7]);
    const std::size_t elut_lines = static_cast<std::size_t>(ctrl_raw[8]);

    if (upper_th_ < lower_th_) {
        throw std::runtime_error("[TLUT_API] control.txt invalido: upper_th debe ser >= lower_th.");
    }

    if (dlut_raw.size() < dlut_lines) {
        throw std::runtime_error("[TLUT_API] dlut.txt no contiene suficientes valores.");
    }

    if (elut_raw.size() < elut_lines) {
        throw std::runtime_error("[TLUT_API] elut.txt no contiene suficientes valores.");
    }

    active_depth_ = static_cast<std::uint32_t>(
        static_cast<std::int32_t>(upper_th_) - static_cast<std::int32_t>(lower_th_) + 1
    );

    const std::size_t dlut_words_needed = ceil_div(dlut_lines, static_cast<std::size_t>(8));
    const std::size_t elut_words_needed = ceil_div(elut_lines, static_cast<std::size_t>(32));

    if (dlut_words_needed > hw_cfg_.dlut_words || elut_words_needed > hw_cfg_.elut_words) {
        throw std::runtime_error("[TLUT_API] Hardware BRAM overflow. Los datos exceden la memoria asignada.");
    }

    auto& bo_d = *bo_d_;
    auto& bo_e = *bo_e_;

    auto* d_map = bo_d.map<uint128_raw*>();
    auto* e_map = bo_e.map<uint128_raw*>();

    std::memset(d_map, 0, hw_cfg_.dlut_words * sizeof(uint128_raw));
    std::memset(e_map, 0, hw_cfg_.elut_words * sizeof(uint128_raw));

    for (std::size_t word_idx = 0; word_idx < dlut_words_needed; ++word_idx) {
        const std::size_t start = word_idx * 8u;
        const std::size_t remaining = dlut_lines - start;
        const std::size_t count = std::min<std::size_t>(8u, remaining);
        d_map[word_idx] = pack_dlut_word(dlut_raw, start, count);
    }

    for (std::size_t word_idx = 0; word_idx < elut_words_needed; ++word_idx) {
        const std::size_t start = word_idx * 32u;
        const std::size_t remaining = elut_lines - start;
        const std::size_t count = std::min<std::size_t>(32u, remaining);
        e_map[word_idx] = pack_elut_word(elut_raw, start, count);
    }

    bo_d.sync(XCL_BO_SYNC_BO_TO_DEVICE, dlut_words_needed * sizeof(uint128_raw), 0);
    bo_e.sync(XCL_BO_SYNC_BO_TO_DEVICE, elut_words_needed * sizeof(uint128_raw), 0);

    tlut_config_t cfg_load{};
    cfg_load.reload_tlut = 1;
    cfg_load.active_depth = active_depth_;

    run_load_->set_arg(4, cfg_load);
    execute_run(*run_load_, last_load_ns_);
}

TlutAccelerator::sample_t* TlutAccelerator::get_in_map() {
    return bo_in_->map<sample_t*>();
}

const TlutAccelerator::sample_t* TlutAccelerator::get_out_map() const {
    return const_cast<xrt::bo*>(bo_out_)->map<sample_t*>();
}

void TlutAccelerator::execute_process(std::size_t samples_count) {
    if (samples_count > hw_cfg_.max_samples) {
        throw std::runtime_error("[TLUT_API] El vector de entrada excede el limite pre-alocado de hardware.");
    }

    auto& bo_in = *bo_in_;
    auto& bo_out = *bo_out_;

    const std::size_t bytes = samples_count * sizeof(sample_t);

    bo_in.sync(XCL_BO_SYNC_BO_TO_DEVICE, bytes, 0);

    tlut_config_t cfg_run{};
    cfg_run.c_sym = c_sym_;
    cfg_run.upper_threshold = upper_th_;
    cfg_run.lower_threshold = lower_th_;
    cfg_run.c_upper = c_upper_;
    cfg_run.c_lower = c_lower_;
    cfg_run.use_sym = use_sym_;
    cfg_run.use_lin = use_lin_;
    cfg_run.num_samples = static_cast<std::uint32_t>(samples_count);
    cfg_run.active_depth = active_depth_;

    run_process_->set_arg(4, cfg_run);
    execute_run(*run_process_, last_compute_ns_);

    bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE, bytes, 0);
}