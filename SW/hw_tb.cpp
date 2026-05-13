/*
 * ----------------------------------------------------------------------------
 * Copyright (c) 2026 Sergio Porras Escobar <sporras29@estudiantec.cr>
 * ----------------------------------------------------------------------------
 */

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

// =========================================================================
// 1. EQUIVALENCIAS Y ESTRUCTURAS
// =========================================================================
const double Q_SCALE = 1024.0; // Precisión Q6.10
const double FPGA_FREQ_MHZ = 250.0; // Frecuencia de reloj del Acelerador

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

// =========================================================================
// 2. REFERENCIAS GOLDEN
// =========================================================================
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

// =========================================================================
// 3. FUNCIÓN PRINCIPAL
// =========================================================================
int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <xclbin> [version]" << std::endl;
        return EXIT_FAILURE;
    }

    std::string version = (argc >= 3) ? argv[2] : "default";
    std::string filename = "hw_results_" + version + ".txt";
    std::ofstream res_file(filename);

    const int HW_MAX_SAMPLES = 100000;
    const int DLUT_WORDS_AXI = 256;  
    const int ELUT_WORDS_AXI = 1024; 
    const int B_SIZE = 16;
    const double SWEEP_STEP = 0.1;
    const double RANGE_PADDING = 1.0;
    const double TOL_HARD = 0.20;

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
            // Parámetros de la función
            int upper = t.control[1];
            int lower = t.control[2];
            int active_depth = (upper - lower) * 1024 + 1;
            int d_cap = (active_depth + B_SIZE - 1) / B_SIZE;

            // Limpieza de memoria (Prevenir Overlap)
            for(int i=0; i<DLUT_WORDS_AXI; i++) d_map[i] = {0,0,0,0};
            for(int i=0; i<ELUT_WORDS_AXI; i++) e_map[i] = {0,0,0,0};

            // Empaquetado de Tablas
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

            // [TRANSACCIÓN 1] Carga de LUTs
            bo_d.sync(XCL_BO_SYNC_BO_TO_DEVICE);
            bo_e.sync(XCL_BO_SYNC_BO_TO_DEVICE);

            nla_config_t cfg = {0};
            cfg.reload_tlut = 1;
            cfg.active_depth = active_depth;
            kernel(bo_in, bo_out, bo_d, bo_e, cfg).wait();

            // Preparación de Estímulos
            int sample_count = 0;
            std::vector<double> x_original;
            for (double x = (double)lower - RANGE_PADDING; x <= (double)upper + RANGE_PADDING; x += SWEEP_STEP) {
                if (sample_count < HW_MAX_SAMPLES) {
                    in_map[sample_count] = (int16_t)(x * Q_SCALE);
                    x_original.push_back(x);
                    sample_count++;
                }
            }
            bo_in.sync(XCL_BO_SYNC_BO_TO_DEVICE);

            // [TRANSACCIÓN 2] Fase de Cómputo
            cfg.reload_tlut = 0;
            cfg.c_sym = (int16_t)(t.control[0] * Q_SCALE);
            cfg.upper_threshold = (int16_t)(upper * Q_SCALE);
            cfg.lower_threshold = (int16_t)(lower * Q_SCALE);
            cfg.c_upper = (int16_t)(t.control[3] * Q_SCALE);
            cfg.c_lower = (int16_t)(t.control[4] * Q_SCALE);
            cfg.use_sym = (uint8_t)t.control[5];
            cfg.use_lin = (uint8_t)t.control[6];
            cfg.num_samples = sample_count;

            auto start = std::chrono::high_resolution_clock::now();
            kernel(bo_in, bo_out, bo_d, bo_e, cfg).wait();
            auto end = std::chrono::high_resolution_clock::now();

            bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

            // ==========================================
            // MÚTRICAS DE RENDIMIENTO HARDWARE (HW Metrics)
            // ==========================================
            double duration_ns = std::chrono::duration<double>(end - start).count() * 1e9;
            double est_cycles = duration_ns * (FPGA_FREQ_MHZ / 1000.0);
            
            // Latencias
            double avg_ns_per_sample = duration_ns / sample_count;
            double avg_cycles_per_sample = est_cycles / sample_count;
            
            // Throughput & Bandwidth
            double throughput_msps = (sample_count / (duration_ns / 1000.0)); // Mega Samples Per Second
            double bytes_transferred = sample_count * 2.0 * 2.0; // IN (16b) + OUT (16b) = 4 Bytes/sample
            double bw_gbs = bytes_transferred / duration_ns; // Ancho de banda efectivo del bus (GB/s)

            // Evaluación de Datos
            double mse = 0.0;
            int local_errors = 0;

            std::stringstream table_out;
            table_out << "X_REAL\tX_HW\tY_HW\tY_IDEAL\tDIFF\tCYCLES/SPL\n";

            for (int j = 0; j < sample_count; j++) {
                double x_val = x_original[j];
                double x_hw  = (double)in_map[j] / Q_SCALE;
                double y_hw  = (double)out_map[j] / Q_SCALE;
                double y_ideal = t.golden(x_val);

                if (x_val > (double)upper || x_val < (double)lower) y_ideal = y_hw; 

                double diff = std::abs(y_hw - y_ideal);
                mse += (diff * diff);
                if (diff > TOL_HARD) local_errors++;

                table_out << std::fixed << std::setprecision(10) 
                          << x_val << "\t" << x_hw << "\t" << y_hw << "\t" << y_ideal << "\t" << diff << "\t" 
                          << std::setprecision(2) << avg_cycles_per_sample << "\n";
            }
            mse /= sample_count;
            total_errors += local_errors;

            // ==========================================
            // IMPRESIÓN (Archivo y Consola)
            // ==========================================
            std::stringstream summary;
            summary << "======================================================\n"
                    << "Función:        " << t.name << "\n"
                    << "Parámetros:     Samples=" << sample_count << " | DLUT=" << d_cap << " | ELUT=" << active_depth << "\n"
                    << "Rendimiento HW: \n"
                    << "  - Frecuencia:      " << FPGA_FREQ_MHZ << " MHz\n"
                    << "  - Latencia Total:  " << std::fixed << std::setprecision(2) << duration_ns << " ns (" << (int)est_cycles << " ciclos)\n"
                    << "  - Latencia/Dato:   " << avg_ns_per_sample << " ns/spl (" << avg_cycles_per_sample << " ciclos/spl)\n"
                    << "  - Throughput:      " << throughput_msps << " MSPS\n"
                    << "  - Ancho de Banda:  " << bw_gbs << " GB/s efectivos\n"
                    << "Precisión:\n"
                    << "  - MSE:             " << std::scientific << std::setprecision(6) << mse << "\n"
                    << "Validación:     " << ((local_errors == 0) ? "[SUCCESS]" : "[FAILED]") << "\n";

            std::cout << summary.str();
            
            res_file << summary.str();
            res_file << "------------------------------------------------------\n";
            res_file << table_out.str() << "\n\n";
        }
        
        std::cout << "======================================================\n";
        std::cout << "FINAL: Errores = " << total_errors << " | Archivo guardado: " << filename << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error Crítico XRT: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}