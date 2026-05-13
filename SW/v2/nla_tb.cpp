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

#include "nla_core.h"
#include "../tluts_B16.h" 

// Funciones Golden de referencia
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

int main() {
    // =========================================================================
    // CONFIGURACIÓN DE PARÁMETROS DE SIMULACIÓN
    // =========================================================================
    const double SWEEP_STEP         = 0.1;   
    const bool   USE_FIXED_RANGE    = false; 
    const double FIXED_RANGE_MIN    = -8.0;  
    const double FIXED_RANGE_MAX    = 8.0;   
    const double RANGE_PADDING      = 1.0;   
    
    const double TOL_SOFT_NORMAL    = 0.05;
    const double TOL_HARD_NORMAL    = 0.15;
    const double TOL_SOFT_LARGE     = 0.15;  
    const double TOL_HARD_LARGE     = 0.40;  

    // Este valor debe coincidir con el 'depth' puesto en el pragma m_axi de in/out_data
    const int HW_MAX_SAMPLES = 100000; 
    // =========================================================================

    std::ofstream log_file("nla_sim.log");
    std::ofstream res_file("nla_results.txt");

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

    // --- DERIVACIÓN AUTOMÁTICA DE TAMAÑOS ---
    // Usamos las constantes del .h para que el TB sea agnóstico a cambios de arquitectura
    axi_word_t d_lut_packed[DLUT_DEPTH / D_RESHAPE_FACTOR];
    axi_word_t e_lut_packed[ELUT_DEPTH / E_RESHAPE_FACTOR];

    // Buffers de datos pre-asignados al tamaño máximo del hardware para evitar SIGSEGV en COSIM
    std::vector<data_t> in_buffer(HW_MAX_SAMPLES, 0);
    std::vector<data_t> out_buffer(HW_MAX_SAMPLES, 0);

    int total_errors = 0;
    int total_warnings = 0;
    double q_scale = (double)(1 << (Q_TOT_WIDTH - Q_INT_WIDTH)); 

    std::cout << "Iniciando validacion Hardware (AXI Memory Mapped)...\n\n";

    for (size_t i = 0; i < tests.size(); i++) {
        TestCase t = tests[i];
        int local_errors = 0;

        int upper = t.control[1];
        int lower = t.control[2];
        int active_depth = (upper - lower) * (int)q_scale + 1;
        int d_cap = (active_depth + B_SIZE - 1) / B_SIZE;

        // --- EMPAQUETADO DINÁMICO ---
        // Empaquetado E-LUT
        for (int c = 0; c < (active_depth + E_RESHAPE_FACTOR - 1) / E_RESHAPE_FACTOR; c++) {
            axi_word_t word = 0;
            for (int j = 0; j < E_RESHAPE_FACTOR; j++) {
                int idx = c * E_RESHAPE_FACTOR + j;
                if (idx < active_depth) 
                    word.range(j * ELUT_WIDTH + (ELUT_WIDTH - 1), j * ELUT_WIDTH) = t.elut[idx];
            }
            e_lut_packed[c] = word;
        }

        // Empaquetado D-LUT
        for (int c = 0; c < (d_cap + D_RESHAPE_FACTOR - 1) / D_RESHAPE_FACTOR; c++) {
            axi_word_t word = 0;
            for (int j = 0; j < D_RESHAPE_FACTOR; j++) {
                int idx = c * D_RESHAPE_FACTOR + j;
                if (idx < d_cap) {
                    data_t val = (float)t.dlut[idx] / q_scale; 
                    word.range(j * DLUT_WIDTH + (DLUT_WIDTH - 1), j * DLUT_WIDTH) = val.range(DLUT_WIDTH - 1, 0);
                }
            }
            d_lut_packed[c] = word;
        }

        // --- TRANSACCIÓN 1: RECARGA DE TABLAS ---
        nla_config_t config_load;
        config_load.reload_tlut = 1;
        config_load.active_depth = active_depth;
        config_load.num_samples = 0; 
        
        // Pasamos in_buffer aunque no se use, para satisfacer el wrapper de COSIM
        nla_top(in_buffer.data(), out_buffer.data(), d_lut_packed, e_lut_packed, config_load);

        // --- PREPARACIÓN DE ESTÍMULOS ---
        double sweep_min = USE_FIXED_RANGE ? FIXED_RANGE_MIN : ((double)lower - RANGE_PADDING);
        double sweep_max = USE_FIXED_RANGE ? FIXED_RANGE_MAX : ((double)upper + RANGE_PADDING);
        
        std::vector<double> x_original; 
        int sample_count = 0;

        for (double x = sweep_min; x <= sweep_max; x += SWEEP_STEP) {
            if (sample_count < HW_MAX_SAMPLES) {
                in_buffer[sample_count] = (data_t)x;
                x_original.push_back(x);
                sample_count++;
            }
        }

        // --- TRANSACCIÓN 2: CÓMPUTO ---
        nla_config_t config_run;
        config_run.c_sym = t.control[0];
        config_run.upper_threshold = t.control[1];
        config_run.lower_threshold = t.control[2];
        config_run.c_upper = t.control[3];
        config_run.c_lower = t.control[4];
        config_run.use_sym = t.control[5];
        config_run.use_lin = t.control[6];
        config_run.reload_tlut = 0;
        config_run.active_depth = active_depth;
        config_run.num_samples = sample_count;

        nla_top(in_buffer.data(), out_buffer.data(), d_lut_packed, e_lut_packed, config_run);

        // --- VALIDACIÓN ---
        res_file << "--- Banco de Resultados: " << t.name << " ---\n";
        for (int j = 0; j < sample_count; j++) {
            double x_val = x_original[j];
            double y_hw = out_buffer[j].to_double();
            double y_expected = t.golden(x_val);

            if (x_val > (double)upper || x_val < (double)lower) y_expected = y_hw; 

            double diff = std::abs(y_hw - y_expected);
            double soft_tol = (std::abs(y_expected) > 10.0) ? TOL_SOFT_LARGE : TOL_SOFT_NORMAL;
            double hard_tol = (std::abs(y_expected) > 10.0) ? TOL_HARD_LARGE : TOL_HARD_NORMAL;

            res_file << std::fixed << std::setprecision(4) << x_val << "\t" << y_hw << "\t" << y_expected << "\t" << diff << "\n";

            if (diff > hard_tol) {
                log_file << "[ERR] " << t.name << " X=" << x_val << " HW=" << y_hw << " Exp=" << y_expected << "\n";
                local_errors++;
                total_errors++;
            } else if (diff > soft_tol) {
                total_warnings++;
            }
        }

        // --- SALIDA A CONSOLA ---
        if (local_errors == 0) {
            std::cout << "  [\u2713] " << t.name << std::endl; 
        } else {
            std::cout << "  [X] " << t.name << " (" << local_errors << " errores)" << std::endl;
        }
    }

    std::cout << "\n------------------------------------------------------\n";
    std::cout << " Warnings: " << total_warnings << " | Errors: " << total_errors << std::endl;
    std::cout << "------------------------------------------------------\n";

    log_file.close(); res_file.close();
    return (total_errors > 0);
}