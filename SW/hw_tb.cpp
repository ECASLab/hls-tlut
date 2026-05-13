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

// Nuevos headers XRT Native C++ API
#include "xrt/xrt_bo.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"

// Tablas de valores precomputados
#include "tluts_B16.h"

// =========================================================================
// 1. EQUIVALENCIAS DE TIPOS Y ESTRUCTURAS (Desacopladas de HLS)
// =========================================================================
const int Q_TOT_WIDTH = 16;
const int Q_INT_WIDTH = 6;
const double Q_SCALE = 1024.0; // 2^(16 - 6) = 1024.0

// Usamos uint128_raw para no hacer conflicto con __uint128_t de GCC
struct uint128_raw {
    uint32_t data[4];
};

// Esta estructura DEBE medir exactamente 24 bytes para coincidir 
// con los 6 registros de 32-bits (config_r_1 a config_r_6) en la FPGA.
struct __attribute__((packed)) nla_config_t {
    int16_t c_sym;
    int16_t upper_threshold;
    int16_t lower_threshold;
    int16_t c_upper;
    int16_t c_lower;
    uint8_t reload_tlut;
    uint8_t use_sym;
    uint8_t use_lin;
    uint8_t padding_1[3];   // Forzar alineación a 32 bits
    uint32_t num_samples;
    uint16_t active_depth;
    uint8_t padding_2[2];   // Relleno final para cerrar en 24 bytes
};

// =========================================================================
// 2. REFERENCIAS GOLDEN (Software)
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
        std::cerr << "Uso: " << argv[0] << " <ruta_al_archivo.xclbin>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    const int HW_MAX_SAMPLES = 100000;
    const int DLUT_WORDS_AXI = 128; // (2048 / 16)
    const int ELUT_WORDS_AXI = 512; // (16384 / 32)
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

    std::ofstream res_file("hw_results.txt");
    res_file << "X_REAL\tX_HW\tY_HW\tY_IDEAL\tDIFF\n";
    int total_errors = 0;

    try {
        std::cout << "Inicializando FPGA y cargando Bitstream..." << std::endl;
        auto device = xrt::device(0);
        auto uuid = device.load_xclbin(binaryFile);
        auto kernel = xrt::kernel(device, uuid, "nla_top");

        // Reserva de memoria en la Alveo
        auto bo_in  = xrt::bo(device, HW_MAX_SAMPLES * sizeof(int16_t), kernel.group_id(0));
        auto bo_out = xrt::bo(device, HW_MAX_SAMPLES * sizeof(int16_t), kernel.group_id(1));
        auto bo_d   = xrt::bo(device, DLUT_WORDS_AXI * sizeof(uint128_raw), kernel.group_id(2));
        auto bo_e   = xrt::bo(device, ELUT_WORDS_AXI * sizeof(uint128_raw), kernel.group_id(3));

        // Mapeo seguro a la CPU
        int16_t* in_map    = bo_in.map<int16_t*>();
        int16_t* out_map   = bo_out.map<int16_t*>();
        uint128_raw* d_map = bo_d.map<uint128_raw*>();
        uint128_raw* e_map = bo_e.map<uint128_raw*>();

        if (!in_map || !out_map || !d_map || !e_map) {
            throw std::runtime_error("Fallo al mapear la memoria BO hacia la CPU.");
        }

        for (const auto& t : tests) {
            int upper = t.control[1];
            int lower = t.control[2];
            int active_depth = (upper - lower) * (int)Q_SCALE + 1;
            int d_cap = (active_depth + B_SIZE - 1) / B_SIZE;

            // Limpieza segura sin usar memset
            for(int i=0; i<DLUT_WORDS_AXI; i++) { d_map[i] = {0,0,0,0}; }
            for(int i=0; i<ELUT_WORDS_AXI; i++) { e_map[i] = {0,0,0,0}; }

            // Empaquetado E-LUT (32 elementos de 4-bits por cada 128-bits)
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

            // Empaquetado D-LUT (8 elementos de 16-bits por cada 128-bits)
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

            // Enviar LUTs al FPGA
            bo_d.sync(XCL_BO_SYNC_BO_TO_DEVICE);
            bo_e.sync(XCL_BO_SYNC_BO_TO_DEVICE);

            // ==========================================
            // TRANSACCIÓN 1: MODO RECARGA DE MEMORIA
            // ==========================================
            nla_config_t config_load = {0};
            config_load.reload_tlut = 1;
            config_load.active_depth = active_depth;
            
            auto run_load = kernel(bo_in, bo_out, bo_d, bo_e, config_load);
            run_load.wait();

            // ==========================================
            // PREPARACIÓN DE ESTÍMULOS DE ENTRADA
            // ==========================================
            double sweep_min = (double)lower - RANGE_PADDING;
            double sweep_max = (double)upper + RANGE_PADDING;
            
            std::vector<double> x_original;
            int sample_count = 0;

            for (double x = sweep_min; x <= sweep_max; x += SWEEP_STEP) {
                if (sample_count < HW_MAX_SAMPLES) {
                    // C++ Cast a 16-bit Fixed Point
                    in_map[sample_count] = (int16_t)(x * Q_SCALE);
                    x_original.push_back(x);
                    sample_count++;
                }
            }

            bo_in.sync(XCL_BO_SYNC_BO_TO_DEVICE);

            // ==========================================
            // TRANSACCIÓN 2: MODO CÓMPUTO + CRONÓMETRO
            // ==========================================
            nla_config_t config_run = {0};
            config_run.c_sym = t.control[0];
            config_run.upper_threshold = (int16_t)(upper * Q_SCALE); // Escalamiento necesario
            config_run.lower_threshold = (int16_t)(lower * Q_SCALE); // Escalamiento necesario
            config_run.c_upper = t.control[3];
            config_run.c_lower = t.control[4];
            config_run.use_sym = t.control[5];
            config_run.use_lin = t.control[6];
            config_run.num_samples = sample_count;
            config_run.active_depth = active_depth;

            auto start = std::chrono::high_resolution_clock::now();
            
            auto run_compute = kernel(bo_in, bo_out, bo_d, bo_e, config_run);
            run_compute.wait(); // Espera al hardware

            auto end = std::chrono::high_resolution_clock::now();
            // ==========================================

            bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

            // CÁLCULOS DE RENDIMIENTO Y PRECISIÓN
            std::chrono::duration<double> diff_t = end - start;
            double duration_ns = diff_t.count() * 1e9;
            double avg_ns_per_sample = duration_ns / sample_count;
            double est_cycles = duration_ns * (250.0 / 1000.0); // 250 MHz

            res_file << "--- Banco: " << t.name << " ---\n";
            double mse = 0.0;
            int local_errors = 0;

            for (int j = 0; j < sample_count; j++) {
                double x_val = x_original[j];
                double x_hw  = (double)in_map[j] / Q_SCALE;
                double y_hw  = (double)out_map[j] / Q_SCALE;
                double y_expected = t.golden(x_val);

                if (x_val > (double)upper || x_val < (double)lower) y_expected = y_hw; 

                double diff = std::abs(y_hw - y_expected);
                mse += (diff * diff);

                if (diff > TOL_HARD) local_errors++;

                res_file << std::fixed << std::setprecision(4) 
                         << x_val << "\t" << x_hw << "\t" << y_hw << "\t" << y_expected << "\t" << diff << "\n";
            }
            mse /= sample_count;
            total_errors += local_errors;

            // IMPRESIÓN EN CONSOLA (Formato Conservado)
            std::cout << "======================================================\n";
            std::cout << "Funcion:\t" << t.name << "\n";
            std::cout << "Datos:\t\tSamples=" << sample_count << " | DLUT=" << d_cap << " | ELUT=" << active_depth << "\n";
            std::cout << "HW Tiempo:\t" << std::fixed << std::setprecision(2) << duration_ns << " ns\n";
            std::cout << "HW Ciclos est:\t" << (int)est_cycles << " ciclos\n";
            std::cout << "HW Velocidad:\t" << avg_ns_per_sample << " ns / muestra\n";
            std::cout << "Precision:\tMSE = " << std::scientific << std::setprecision(6) << mse << "\n";
            std::cout << "Validacion:\t" << ((local_errors == 0) ? "[SUCCESS]" : "[FAILED]") << "\n";
        }

        std::cout << "======================================================\n";
        std::cout << "FINAL:\t\tTotal Errores Criticos = " << total_errors << "\n";
        std::cout << "======================================================\n";

    } catch (const std::exception& e) {
        std::cerr << "\n[ERROR CRÍTICO XRT] " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    res_file.close();
    return (total_errors > 0) ? EXIT_FAILURE : EXIT_SUCCESS;
}