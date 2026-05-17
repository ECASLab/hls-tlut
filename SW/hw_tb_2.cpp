#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <sstream>

#include "xrt/xrt_bo.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"
#include "tluts_B16.h"

const double Q_SCALE = 1024.0;
const double FPGA_FREQ_MHZ = 250.0;

struct uint128_raw {
    uint32_t data[4];
};

struct __attribute__((packed)) nla_config_t {
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

double g_sigmoid(double x)  { return 1.0 / (1.0 + std::exp(-x)); }
double g_tanh(double x)     { return std::tanh(x); }
double g_softsign(double x) { return x / (1.0 + std::abs(x)); }
double g_erf(double x)      { return std::erf(x); }
double g_swish(double x)    { return x / (1.0 + std::exp(-x)); }
double g_gelu(double x)     { return 0.5 * x * (1.0 + std::erf(x / std::sqrt(2.0))); }
double g_softplus(double x) { return std::log(1.0 + std::exp(x)); }
double g_mish(double x)     { return x * std::tanh(std::log(1.0 + std::exp(x))); }
double g_elu(double x)      { return (x < 0.0) ? (std::exp(x) - 1.0) : x; }
double g_exp(double x)      { return std::exp(x); }
double g_sqrt(double x)     { return (x >= 0.0) ? std::sqrt(x) : 0.0; }
double g_relu(double x)     { return (x > 0.0) ? x : 0.0; }

struct TestCase {
    std::string name;
    const int* dlut;
    const int* elut;
    const int* control;
    double (*golden)(double);
};

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <xclbin> [version]" << std::endl;
        return EXIT_FAILURE;
    }

    std::string version = (argc >= 3) ? argv[2] : "default";
    std::string filename_res = "hw_results_" + version + ".txt";
    std::string filename_log = "hw_interaction_" + version + ".log";
    std::ofstream res_file(filename_res);
    std::ofstream log_file(filename_log);

    const int HW_MAX_SAMPLES = 100000;
    const int DLUT_WORDS_AXI = 256;  
    const int ELUT_WORDS_AXI = 1024; 
    const int B_SIZE = 16;
    const double SWEEP_STEP = 0.2;
    const double RANGE_PADDING = 1.0;
    const double TOL_HARD = 0.20;

    const bool SWEEP_MODE_FIXED = true;
    const double SWEEP_FIXED_MIN = -12.0;
    const double SWEEP_FIXED_MAX = 12.0;

    std::vector<TestCase> tests = {
        {"SIGMOID", SIGMOID_DLUT, SIGMOID_ELUT, SIGMOID_CONTROL, g_sigmoid},
        {"TANH", TANH_DLUT, TANH_ELUT, TANH_CONTROL, g_tanh},
        {"SOFTSIGN", SOFTSIGN_DLUT, SOFTSIGN_ELUT, SOFTSIGN_CONTROL, g_softsign},
        {"ERF", ERF_DLUT, ERF_ELUT, ERF_CONTROL, g_erf},
        {"SWISH", SWISH_DLUT, SWISH_ELUT, SWISH_CONTROL, g_swish},
        {"GELU", GELU_DLUT, GELU_ELUT, GELU_CONTROL, g_gelu},
        {"SOFTPLUS", SOFTPLUS_DLUT, SOFTPLUS_ELUT, SOFTPLUS_CONTROL, g_softplus},
        {"MISH", MISH_DLUT, MISH_ELUT, MISH_CONTROL, g_mish},
        {"ELU", ELU_DLUT, ELU_ELUT, ELU_CONTROL, g_elu},
        {"EXP", EXP_DLUT, EXP_ELUT, EXP_CONTROL, g_exp},
        {"SQRT", SQRT_DLUT, SQRT_ELUT, SQRT_CONTROL, g_sqrt},
        {"RELU", RELU_DLUT, RELU_ELUT, RELU_CONTROL, g_relu}
    };

    try {
        auto device = xrt::device(0);
        auto uuid = device.load_xclbin(argv[1]);
        auto kernel = xrt::kernel(device, uuid, "nla_top");

        auto bo_in  = xrt::bo(device, HW_MAX_SAMPLES * sizeof(int16_t), kernel.group_id(0));
        auto bo_out = xrt::bo(device, HW_MAX_SAMPLES * sizeof(int16_t), kernel.group_id(1));
        auto bo_d   = xrt::bo(device, DLUT_WORDS_AXI * sizeof(uint128_raw), kernel.group_id(2));
        auto bo_e   = xrt::bo(device, ELUT_WORDS_AXI * sizeof(uint128_raw), kernel.group_id(3));

        int16_t* in_map    = bo_in.map<int16_t*>();
        int16_t* out_map   = bo_out.map<int16_t*>();
        uint128_raw* d_map = bo_d.map<uint128_raw*>();
        uint128_raw* e_map = bo_e.map<uint128_raw*>();

        int total_errors = 0;

        for (const auto& t : tests) {
            int upper_scaled = t.control[1];
            int lower_scaled = t.control[2];
            double upper_d = (double)upper_scaled / Q_SCALE;
            double lower_d = (double)lower_scaled / Q_SCALE;

            int active_depth = (upper_scaled - lower_scaled) + 1;
            int d_cap = (active_depth + B_SIZE - 1) / B_SIZE;

            for(int i=0; i<DLUT_WORDS_AXI; i++) d_map[i] = {0,0,0,0};
            for(int i=0; i<ELUT_WORDS_AXI; i++) e_map[i] = {0,0,0,0};

            for (int c = 0; c < (active_depth + 31) / 32; c++) {
                uint128_raw word = {0, 0, 0, 0};
                for (int j = 0; j < 32; j++) {
                    int idx = c * 32 + j;
                    if (idx < active_depth) {
                        uint32_t val = t.elut[idx] & 0xF;
                        word.data[j / 8] |= (val << ((j * 4) % 32));
                    }
                }
                e_map[c] = word;
            }

            for (int c = 0; c < (d_cap + 7) / 8; c++) {
                uint128_raw word = {0, 0, 0, 0};
                for (int j = 0; j < 8; j++) {
                    int idx = c * 8 + j;
                    if (idx < d_cap) {
                        uint32_t val = (uint16_t)t.dlut[idx]; 
                        word.data[j / 2] |= (val << ((j * 16) % 32));
                    }
                }
                d_map[c] = word;
            }

            bo_d.sync(XCL_BO_SYNC_BO_TO_DEVICE);
            bo_e.sync(XCL_BO_SYNC_BO_TO_DEVICE);

            nla_config_t cfg_load = {0};
            cfg_load.reload_tlut = 1;
            cfg_load.active_depth = active_depth;

            auto start_load = std::chrono::high_resolution_clock::now();
            kernel(bo_in, bo_out, bo_d, bo_e, cfg_load).wait();
            auto end_load = std::chrono::high_resolution_clock::now();

            double load_duration_ns = std::chrono::duration<double>(end_load - start_load).count() * 1e9;
            double load_est_cycles = load_duration_ns * (FPGA_FREQ_MHZ / 1000.0);

            int sample_count = 0;
            std::vector<double> x_original;
            
            double sweep_start = SWEEP_MODE_FIXED ? SWEEP_FIXED_MIN : (lower_d - RANGE_PADDING);
            double sweep_end   = SWEEP_MODE_FIXED ? SWEEP_FIXED_MAX : (upper_d + RANGE_PADDING);

            for (double x = sweep_start; x <= sweep_end; x += SWEEP_STEP) {
                if (sample_count < HW_MAX_SAMPLES) {
                    in_map[sample_count] = (int16_t)(x * Q_SCALE);
                    x_original.push_back(x);
                    sample_count++;
                }
            }
            bo_in.sync(XCL_BO_SYNC_BO_TO_DEVICE);

            nla_config_t cfg_run = {0};
            cfg_run.reload_tlut = 0;
            cfg_run.c_sym = (int16_t)t.control[0];
            cfg_run.upper_threshold = (int16_t)upper_scaled;
            cfg_run.lower_threshold = (int16_t)lower_scaled;
            cfg_run.c_upper = (int16_t)t.control[3];
            cfg_run.c_lower = (int16_t)t.control[4];
            cfg_run.use_sym = (uint8_t)t.control[5];
            cfg_run.use_lin = (uint8_t)t.control[6];
            cfg_run.num_samples = sample_count;

            auto start_comp = std::chrono::high_resolution_clock::now();
            kernel(bo_in, bo_out, bo_d, bo_e, cfg_run).wait();
            auto end_comp = std::chrono::high_resolution_clock::now();

            bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

            double comp_duration_ns = std::chrono::duration<double>(end_comp - start_comp).count() * 1e9;
            double comp_est_cycles = comp_duration_ns * (FPGA_FREQ_MHZ / 1000.0);
            double avg_ns_per_sample = comp_duration_ns / sample_count;
            double avg_cycles_per_sample = comp_est_cycles / sample_count;

            double mse = 0.0;
            double mne = 0.0; 
            int local_errors = 0;
            int valid_samples_in_range = 0;

            std::stringstream table_out;
            table_out << "X_REAL\tX_HW\tY_HW\tY_IDEAL\tDIFF\n";

            log_file << "======================================================\n";
            log_file << "[HW_LOG] INICIANDO TRANSACCIÓN: " << t.name << "\n";
            log_file << "[HW_LOG] CONFIG AXI-LITE: reload_tlut=" << (int)cfg_run.reload_tlut 
                     << ", c_sym=" << cfg_run.c_sym 
                     << ", upper=" << cfg_run.upper_threshold 
                     << ", lower=" << cfg_run.lower_threshold 
                     << ", use_sym=" << (int)cfg_run.use_sym << "\n";
            log_file << "[HW_LOG] FASE 1 - LUT LOAD: " << load_est_cycles << " ciclos (" << load_duration_ns << " ns)\n";
            log_file << "[HW_LOG] FASE 2 - COMPUTE INICIO: Procesando " << sample_count << " datos...\n";

            for (int j = 0; j < sample_count; j++) {
                double x_val = x_original[j];
                double x_hw  = (double)in_map[j] / Q_SCALE;
                double y_hw  = (double)out_map[j] / Q_SCALE;
                
                double y_ideal = t.golden(x_val);
                double diff = std::abs(y_hw - y_ideal);

                if (x_val >= lower_d && x_val <= upper_d) {
                    mse += (diff * diff);
                    if (y_ideal != 0.0) {
                        mne += (diff / std::abs(y_ideal)); 
                    } else {
                        mne += diff; 
                    }
                    valid_samples_in_range++;
                    if (diff > TOL_HARD) local_errors++;
                }

                table_out << std::fixed << std::setprecision(10) 
                          << x_val << "\t" << x_hw << "\t" << y_hw << "\t" << y_ideal << "\t" << diff << "\n";
                          
                log_file << "[HW_LOG] SPL[" << j << "] X=" << x_val 
                         << " | Y_HW=" << y_hw << " | Y_ID=" << y_ideal 
                         << " | STATUS=" << ((diff > TOL_HARD && x_val >= lower_d && x_val <= upper_d) ? "SATURADO" : "OK") << "\n";
            }
            
            if (valid_samples_in_range > 0) {
                mse /= valid_samples_in_range;
                mne /= valid_samples_in_range;
            }
            total_errors += local_errors;

            log_file << "[HW_LOG] FASE 2 - COMPUTE FIN: " << comp_est_cycles << " ciclos totales.\n\n";

            std::stringstream summary;
            summary << "======================================================\n"
                    << "Función:        " << t.name << "\n"
                    << "Parámetros:     Samples=" << sample_count << " | DLUT=" << d_cap << " | ELUT=" << active_depth << "\n"
                    << "Tiempos Transacción 1 (LUT LOAD):\n"
                    << "  - Latencia Total:  " << std::fixed << std::setprecision(2) << load_duration_ns << " ns (" << (int)load_est_cycles << " ciclos)\n"
                    << "Tiempos Transacción 2 (COMPUTE):\n"
                    << "  - Latencia Total:  " << std::fixed << std::setprecision(2) << comp_duration_ns << " ns (" << (int)comp_est_cycles << " ciclos)\n"
                    << "  - Procesamiento:   Average: " << avg_ns_per_sample << " ns/spl (" << std::setprecision(2) << avg_cycles_per_sample << " ciclos/spl)\n"
                    << "Precisión (Rango [" << lower_d << ", " << upper_d << "]):\n"
                    << "  - MSE:             " << std::scientific << std::setprecision(6) << mse << "\n"
                    << "  - MNE:             " << std::scientific << std::setprecision(6) << mne << "\n"
                    << "Validación:     " << ((local_errors == 0) ? "[SUCCESS]" : "[FAILED]") << "\n";

            std::cout << summary.str();
            
            res_file << summary.str();
            res_file << "------------------------------------------------------\n";
            res_file << table_out.str() << "\n\n";
        }
        
        std::cout << "======================================================\n";
        std::cout << "FINAL: Errores HW = " << total_errors << "\n"
                  << " - Resumen: " << filename_res << "\n"
                  << " - Datalog: " << filename_log << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error Crítico XRT: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    res_file.close();
    log_file.close();
    return EXIT_SUCCESS;
}