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
#include <sstream>

#include "api/tlut_api.cpp"

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
    std::string name_print;
    std::string folder_name;
    double lower_th;
    double upper_th;
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

    const double SWEEP_STEP = 0.024;
    const double RANGE_PADDING = 1.0;
    const double TOL_HARD = 0.10;

    const bool SWEEP_MODE_FIXED = true;
    const double SWEEP_FIXED_MIN = -12.0;
    const double SWEEP_FIXED_MAX = 12.0;

    std::vector<TestCase> tests = {
        {"SIGMOID",  "sigmoid",  -6.0,  6.0, g_sigmoid},
        {"TANH",     "tanh",     -4.0,  4.0, g_tanh},
        {"SOFTSIGN", "softsign", -8.0,  8.0, g_softsign},
        {"ERF",      "erf",      -3.0,  3.0, g_erf},
        {"SWISH",    "swish",    -6.0,  6.0, g_swish},
        {"GELU",     "gelu",     -4.0,  4.0, g_gelu},
        {"SOFTPLUS", "softplus", -5.0,  4.0, g_softplus},
        {"MISH",     "mish",     -5.0,  3.0, g_mish},
        {"ELU",      "elu",      -6.0,  0.0, g_elu},
        {"EXP",      "exp",      -1.0,  1.0, g_exp},
        {"SQRT",     "sqrt",      0.0, 10.0, g_sqrt},
        {"RELU",     "relu",     -1.0,  1.0, g_relu}
    };

    try {
        TlutHardwareConfig hw_cfg;
        hw_cfg.enable_profiling = true;

        TlutAccelerator accel(argv[1], 6, 10, hw_cfg, 0);

        // ==========================================================
        // WARM-UP TRANSACTION: Absorber latencia de inicialización PCIe
        // ==========================================================
        std::cout << "[INFO] Realizando warm-up del bus PCIe y Driver XRT..." << std::endl;
        try {
            accel.load(tests[0].folder_name);
            std::vector<float> dummy_data(256, 0.0f);
            accel.process(dummy_data);
        } catch (const std::exception& e) {
            std::cerr << "[WARNING] Excepción durante warm-up: " << e.what() << std::endl;
        }
        std::cout << "[INFO] Warm-up completado. Iniciando perfilado de latencias puras HW.\n" << std::endl;
        // ==========================================================

        int total_errors = 0;

        for (const auto& t : tests) {
            double sweep_start = SWEEP_MODE_FIXED ? SWEEP_FIXED_MIN : (t.lower_th - RANGE_PADDING);
            double sweep_end   = SWEEP_MODE_FIXED ? SWEEP_FIXED_MAX : (t.upper_th + RANGE_PADDING);

            std::vector<float> x_original;
            for (double x = sweep_start; x <= sweep_end; x += SWEEP_STEP) {
                if (x_original.size() < hw_cfg.max_samples) {
                    x_original.push_back(static_cast<float>(x));
                }
            }
            
            size_t sample_count = x_original.size();

            // LUT Load
            accel.load(t.folder_name);
            double load_duration_ns = accel.get_last_load_duration_ns();
            double load_est_cycles = load_duration_ns * (hw_cfg.fpga_freq_mhz / 1000.0);

            // Inference Compute
            std::vector<float> y_hw = accel.process(x_original);
            double comp_duration_ns = accel.get_last_compute_duration_ns();
            double comp_est_cycles = comp_duration_ns * (hw_cfg.fpga_freq_mhz / 1000.0);
            
            double avg_ns_per_sample = comp_duration_ns / sample_count;
            double avg_cycles_per_sample = comp_est_cycles / sample_count;

            double mse = 0.0;
            double mne = 0.0; 
            int local_errors = 0;
            int valid_samples_in_range = 0;

            std::stringstream error_details;

            log_file << "======================================================\n";
            log_file << "[HW_LOG] INICIANDO TRANSACCIÓN: " << t.name_print << "\n";
            log_file << "[HW_LOG] FASE 1 - LUT LOAD (Puro HW): " << load_est_cycles << " ciclos (" << load_duration_ns << " ns)\n";
            log_file << "[HW_LOG] FASE 2 - COMPUTE INICIO: Procesando " << sample_count << " datos...\n";

            for (size_t j = 0; j < sample_count; j++) {
                double x_val = x_original[j];
                double y_val_hw = y_hw[j];
                double y_ideal = t.golden(x_val);
                double diff = std::abs(y_val_hw - y_ideal);

                if (x_val >= t.lower_th && x_val <= t.upper_th) {
                    mse += (diff * diff);
                    if (y_ideal != 0.0) {
                        mne += (diff / std::abs(y_ideal)); 
                    } else {
                        mne += diff; 
                    }
                    valid_samples_in_range++;
                    
                    // Manejo inteligente de log de errores (Máximo 10)
                    if (diff > TOL_HARD) {
                        local_errors++;
                        if (local_errors <= 10) {
                            error_details << "  -> Error #" << local_errors 
                                          << " | X=" << std::fixed << std::setprecision(5) << x_val 
                                          << " | Y_HW=" << y_val_hw 
                                          << " | Y_IDEAL=" << y_ideal 
                                          << " | DIFF=" << diff << "\n";
                        }
                    }
                }
            }
            
            if (valid_samples_in_range > 0) {
                mse /= valid_samples_in_range;
                mne /= valid_samples_in_range;
            }
            total_errors += local_errors;

            // Volcado de estado al Datalog
            if (local_errors > 0) {
                log_file << "[HW_LOG] ADVERTENCIA: Se detectaron " << local_errors << " muestras fuera del límite de tolerancia (TOL_HARD).\n";
                log_file << "[HW_LOG] Detalles de las primeras " << std::min(local_errors, 10) << " desviaciones detectadas:\n";
                log_file << error_details.str();
            } else {
                log_file << "[HW_LOG] ESTADO: Señal íntegra. Precisión validada sin exceder tolerancia.\n";
            }
            
            log_file << "[HW_LOG] FASE 2 - COMPUTE FIN: " << comp_est_cycles << " ciclos de hardware totales.\n\n";

            // Resumen Ejecutivo a la Consola y al Resumen de Resultados
            std::stringstream summary;
            summary << "======================================================\n"
                    << "Función:        " << t.name_print << "\n"
                    << "Parámetros:     Samples=" << sample_count << "\n"
                    << "Tiempos Transacción 1 (LUT LOAD - Sólo HW):\n"
                    << "  - Latencia Total:  " << std::fixed << std::setprecision(2) << load_duration_ns << " ns (" << (int)load_est_cycles << " ciclos)\n"
                    << "Tiempos Transacción 2 (COMPUTE - Sólo HW):\n"
                    << "  - Latencia Total:  " << std::fixed << std::setprecision(2) << comp_duration_ns << " ns (" << (int)comp_est_cycles << " ciclos)\n"
                    << "  - Procesamiento:   Average: " << avg_ns_per_sample << " ns/spl (" << std::setprecision(2) << avg_cycles_per_sample << " ciclos/spl)\n"
                    << "Precisión (Rango [" << t.lower_th << ", " << t.upper_th << "]):\n"
                    << "  - MSE:             " << std::scientific << std::setprecision(6) << mse << "\n"
                    << "  - MNE:             " << std::scientific << std::setprecision(6) << mne << "\n"
                    << "Validación:     " << ((local_errors == 0) ? "[SUCCESS]" : "[FAILED] (" + std::to_string(local_errors) + " Errores)") << "\n";

            std::cout << summary.str();
            res_file << summary.str();
        }
        
        std::cout << "======================================================\n";
        std::cout << "FINAL: Errores Totales HW = " << total_errors << "\n"
                  << " - Resumen: " << filename_res << "\n"
                  << " - Datalog detallado: " << filename_log << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error Crítico: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    res_file.close();
    log_file.close();
    return EXIT_SUCCESS;
}