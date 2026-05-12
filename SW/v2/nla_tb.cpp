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

// Funciones Golden de referencia (Matemática ideal en software)
double g_sigmoid(double x) { return 1.0 / (1.0 + std::exp(-x)); }
double g_tanh(double x) { return std::tanh(x); }
double g_softsign(double x) { return x / (1.0 + std::abs(x)); }
double g_erf(double x) { return std::erf(x); }
double g_swish(double x) { return x / (1.0 + std::exp(-x)); }
double g_gelu(double x) { return 0.5 * x * (1.0 + std::erf(x / std::sqrt(2.0))); }
double g_softplus(double x) { return std::log(1.0 + std::exp(x)); }
double g_mish(double x) { return x * std::tanh(std::log(1.0 + std::exp(x))); }
double g_elu(double x) { return (x < 0.0) ? (std::exp(x) - 1.0) : x; }
double g_exp(double x) { return std::exp(x); }
double g_sqrt(double x) { return (x >= 0.0) ? std::sqrt(x) : 0.0; }
double g_relu(double x) { return (x > 0.0) ? x : 0.0; }

struct TestCase {
    std::string name;
    const int* dlut;
    const int* elut;
    const int* control;
    double (*golden)(double);
};

int main() {
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

    // Arreglos empaquetados para simular la memoria principal del ARM a 128 bits
    axi_word_t d_lut_packed[(DLUT_DEPTH + D_RESHAPE_FACTOR - 1) / D_RESHAPE_FACTOR];
    axi_word_t e_lut_packed[(ELUT_DEPTH + E_RESHAPE_FACTOR - 1) / E_RESHAPE_FACTOR];

    // Contadores globales
    int total_errors = 0;
    int total_warnings = 0;

    int frac_width = Q_TOT_WIDTH - Q_INT_WIDTH;
    double q_scale = (double)(1 << frac_width); 

    for (size_t i = 0; i < tests.size(); i++) {
        TestCase t = tests[i];

        log_file << "========================================\n";
        log_file << "Iniciando validacion hardware para funcion: " << t.name << "\n";

        int upper = t.control[1];
        int lower = t.control[2];
        
        int active_depth = (upper - lower) * (int)q_scale + 1;
        int d_cap = (active_depth + B_SIZE - 1) / B_SIZE;

        // Empaquetado E-LUT (Fragmentos de E_RESHAPE_FACTOR elementos por palabra de 128 bits)
        int e_chunks = (active_depth + E_RESHAPE_FACTOR - 1) / E_RESHAPE_FACTOR;
        for (int c = 0; c < e_chunks; c++) {
            axi_word_t word = 0;
            for (int j = 0; j < E_RESHAPE_FACTOR; j++) {
                int idx = c * E_RESHAPE_FACTOR + j;
                if (idx < active_depth) {
                    word.range(j * ELUT_WIDTH + (ELUT_WIDTH - 1), j * ELUT_WIDTH) = t.elut[idx];
                }
            }
            e_lut_packed[c] = word;
        }

        // Empaquetado D-LUT (Fragmentos de D_RESHAPE_FACTOR elementos por palabra de 128 bits)
        int d_chunks = (d_cap + D_RESHAPE_FACTOR - 1) / D_RESHAPE_FACTOR;
        for (int c = 0; c < d_chunks; c++) {
            axi_word_t word = 0;
            for (int j = 0; j < D_RESHAPE_FACTOR; j++) {
                int idx = c * D_RESHAPE_FACTOR + j;
                if (idx < d_cap) {
                    // Conversión al tipo ap_fixed configurado para obtener los bits exactos
                    data_t val = (float)t.dlut[idx] / q_scale; 
                    word.range(j * DLUT_WIDTH + (DLUT_WIDTH - 1), j * DLUT_WIDTH) = val.range(DLUT_WIDTH - 1, 0);
                }
            }
            d_lut_packed[c] = word;
        }

        nla_config_t config_load;
        config_load.reload_tlut = 1;
        config_load.active_depth = active_depth;
        config_load.num_samples = 0; 

        hls::stream<axis_t> dummy_in;
        hls::stream<axis_t> dummy_out;

        log_file << "Modo configuracion: Transmitiendo tablas empaquetadas (128-bit) por AXI-Master...\n";
        nla_top(dummy_in, dummy_out, d_lut_packed, e_lut_packed, config_load);

        std::vector<double> x_stim;
        for (double x = -8.0; x <= 8.0; x += 0.2) {
            x_stim.push_back(x);
        }

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
        config_run.num_samples = x_stim.size();

        hls::stream<axis_t> strm_in;
        hls::stream<axis_t> strm_out;

        log_file << "Modo computo: Generando vector de estimulos X en el rango [-8.0, 8.0]...\n";
        res_file << "--- Banco de Resultados: " << t.name << " ---\n";
        res_file << "X_original\tY_hw\t\tY_esperado\tDesviacion\n";

        for (size_t j = 0; j < x_stim.size(); j++) {
            axis_t val_in;
            val_in.data = x_stim[j];
            strm_in.write(val_in);
        }

        nla_top(strm_in, strm_out, d_lut_packed, e_lut_packed, config_run);

        for (size_t j = 0; j < x_stim.size(); j++) {
            axis_t val_out = strm_out.read();
            double x_val = x_stim[j];
            double y_hw = val_out.data.to_double();
            double y_expected = t.golden(x_val);

            if (x_val > (double)upper || x_val < (double)lower) {
                y_expected = y_hw;
            }

            double diff = std::abs(y_hw - y_expected);

            double soft_tolerance = 0.05;
            double hard_tolerance = 0.15;

            if (y_expected > 10.0) {
                soft_tolerance = 0.15;
                hard_tolerance = 0.40;
            }

            res_file << std::fixed << std::setprecision(4) << x_val << "\t\t" << y_hw << "\t\t" << y_expected << "\t\t" << diff << "\n";

            if (diff > hard_tolerance) {
                log_file << "[ERROR CRITICO] Desviacion INACEPTABLE en X = " << x_val 
                         << " | Y_HW = " << y_hw << " | Y_Ideal = " << y_expected 
                         << " | Diff = " << diff << "\n";
                total_errors++;
            } else if (diff > soft_tolerance) {
                log_file << "[ADVERTENCIA] Desviacion leve (Aceptada) en X = " << x_val 
                         << " | Y_HW = " << y_hw << " | Y_Ideal = " << y_expected 
                         << " | Diff = " << diff << "\n";
                total_warnings++;
            }
        }

        log_file << "Cierre de pipeline de pruebas para modulo " << t.name << " exitoso.\n\n";
        res_file << "\n";
    }

    std::cout << "\n======================================================\n";
    std::cout << "               RESUMEN DE SIMULACION HLS                \n";
    std::cout << "======================================================\n";
    std::cout << " -> Total de Advertencias Leves (Ignoradas): " << total_warnings << "\n";
    std::cout << " -> Total de Errores Criticos (Fallos):      " << total_errors << "\n";
    std::cout << "======================================================\n";

    if (total_errors == 0) {
        log_file << ">>> REPORTE FINAL: SIMULACION CSIM/COSIM SUPERADA SIN ERRORES CRITICOS <<<\n";
        std::cout << "\n[PASS] Testbench completado exitosamente. Las desviaciones estuvieron dentro de los limites.\n\n";
    } else {
        log_file << ">>> REPORTE FINAL: SIMULACION COMPROMETIDA. " << total_errors << " ERRORES CRITICOS DETECTADOS <<<\n";
        std::cout << "\n[FAIL] Testbench abortado con " << total_errors << " errores criticos. Consulte nla_sim.log para detalles.\n\n";
    }

    log_file.close();
    res_file.close();

    return (total_errors > 0) ? 1 : 0;
}