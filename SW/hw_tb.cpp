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
#include <limits>
#include <locale>

#include "api/tlut_api.cpp"

// ----------------------------------------------------------------------------
// Reglas de Formato (Costa Rica / RAE)
// Decimales con coma (,), sin separador de miles.
// ----------------------------------------------------------------------------
struct FormatoCR : std::numpunct<char> {
    char do_decimal_point() const override { return ','; }
    std::string do_grouping() const override { return ""; }
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
    std::string name_print;
    std::string folder_name;
    double lower_th;
    double upper_th;
    double (*golden)(double);
};

int main(int argc, char **argv) {
    // Aplicamos el formato RAE globalmente a la consola estándar
    std::locale localeCR(std::locale::classic(), new FormatoCR);
    std::cout.imbue(localeCR);
    std::cerr.imbue(localeCR);

    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <xclbin> [version] [num_samples]" << std::endl;
        return EXIT_FAILURE;
    }

    std::string version = (argc >= 3) ? argv[2] : "default";
    size_t num_samples = (argc >= 4) ? std::stoul(argv[3]) : 10000;

    std::string filename_res = "hw_results_" + version + ".txt";
    std::string filename_log = "hw_interaction_" + version + ".log";
    std::ofstream res_file(filename_res);
    std::ofstream log_file(filename_log);

    // Aplicamos el formato RAE a los archivos de salida
    res_file.imbue(localeCR);
    log_file.imbue(localeCR);

    const double RANGE_PADDING = 1.0;
    const double TOL_HARD = 0.15;

    const bool SWEEP_MODE_FIXED = true;
    const double SWEEP_FIXED_MIN = -16.0;
    const double SWEEP_FIXED_MAX = 16.0;
    
    // Iteraciones para filtrar el Jitter del SO y obtener latencia pura de HW
    const int PROFILING_ITERATIONS = 5;

    std::vector<TestCase> tests = {
        {"SIGMOID",  "sigmoid",     -6.0,   6.0, g_sigmoid},
        {"TANH",     "tanh",        -4.0,   4.0, g_tanh},
        {"SOFTSIGN", "softsign",    -8.0,   8.0, g_softsign},
        {"ERF",      "erf",         -3.0,   3.0, g_erf},
        {"SWISH",    "swish",       -6.0,   6.0, g_swish},
        {"GELU",     "gelu",        -4.0,   4.0, g_gelu},
        {"SOFTPLUS", "softplus",    -5.0,   4.0, g_softplus},
        {"MISH",     "mish",        -5.0,   3.0, g_mish},
        {"ELU",      "elu",         -6.0,   0.0, g_elu},
        {"EXP",      "exp",         -8.0,   1.0, g_exp},
        {"SQRT",     "sqrt",         0.0,  15.875, g_sqrt},
        {"RELU",     "relu",        -0.125, 0.125, g_relu}
    };

    try {
        TlutHardwareConfig hw_cfg;
        hw_cfg.enable_profiling = true;
        // Ajustamos dinámicamente el tamaño del HW si el usuario pide muchos datos
        if (num_samples > hw_cfg.max_samples) hw_cfg.max_samples = num_samples; 

        // Inicialización de la API del acelerador
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
            std::cerr << "[WARNING] Excepcion durante warm-up: " << e.what() << std::endl;
        }
        std::cout << "[INFO] Warm-up completado. Iniciando perfilado de latencias puras HW.\n" << std::endl;
        // ==========================================================

        int total_errors = 0;

        for (const auto& t : tests) {
            double sweep_start = SWEEP_MODE_FIXED ? SWEEP_FIXED_MIN : (t.lower_th - RANGE_PADDING);
            double sweep_end   = SWEEP_MODE_FIXED ? SWEEP_FIXED_MAX : (t.upper_th + RANGE_PADDING);

            // Generación de exactamente 'num_samples' mediante interpolación
            double sweep_step = (num_samples > 1) ? (sweep_end - sweep_start) / (num_samples - 1) : 0.0;

            std::vector<float> x_original;
            x_original.reserve(num_samples);
            for (size_t i = 0; i < num_samples; ++i) {
                double x_val = sweep_start + i * sweep_step;
                x_original.push_back(static_cast<float>(x_val));
            }
            
            size_t sample_count = x_original.size();

            // LUT Load con filtro de Jitter
            // Verificado: Toma exclusivamente el valor mínimo de las iteraciones
            double load_duration_ns = std::numeric_limits<double>::max();
            for(int i = 0; i < PROFILING_ITERATIONS; ++i) {
                accel.load(t.folder_name);
                double current_ns = accel.get_last_load_duration_ns();
                if(current_ns < load_duration_ns) load_duration_ns = current_ns;
            }
            double load_est_cycles = load_duration_ns * (hw_cfg.fpga_freq_mhz / 1000.0);

            // Inference Compute con filtro de Jitter
            // Verificado: Toma exclusivamente el valor mínimo de las iteraciones
            double comp_duration_ns = std::numeric_limits<double>::max();
            std::vector<float> y_hw;
            for(int i = 0; i < PROFILING_ITERATIONS; ++i) {
                y_hw = accel.process(x_original); // Validaremos los datos de la última iteración
                double current_ns = accel.get_last_compute_duration_ns();
                if(current_ns < comp_duration_ns) comp_duration_ns = current_ns;
            }
            double comp_est_cycles = comp_duration_ns * (hw_cfg.fpga_freq_mhz / 1000.0);
            
            // Cálculos de Rendimiento (Throughput)
            double throughput_msps = (static_cast<double>(sample_count) / comp_duration_ns) * 1000.0;
            double avg_ns_per_sample = comp_duration_ns / sample_count;
            double avg_cycles_per_sample = comp_est_cycles / sample_count;

            double mse = 0.0;
            double mne = 0.0; 
            double mae = 0.0; // MAE (Maximum Absolute Error)
            int local_errors = 0;
            int valid_samples_in_range = 0;

            std::stringstream error_details;
            error_details.imbue(localeCR); // Formato RAE a los detalles

            log_file << "======================================================\n";
            log_file << "[HW_LOG] INICIANDO TRANSACCION: " << t.name_print << "\n";
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
                    
                    // Cálculo de MAE (Maximum Absolute Error)
                    if (diff > mae) {
                        mae = diff;
                    }
                    
                    valid_samples_in_range++;
                    
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

            if (local_errors > 0) {
                log_file << "[HW_LOG] ADVERTENCIA: Se detectaron " << local_errors << " muestras fuera del limite de tolerancia (TOL_HARD).\n";
                log_file << "[HW_LOG] Detalles de las primeras " << std::min(local_errors, 10) << " desviaciones detectadas:\n";
                log_file << error_details.str();
            } else {
                log_file << "[HW_LOG] ESTADO: Senal integra. Precision validada sin exceder tolerancia.\n";
            }
            
            log_file << "[HW_LOG] FASE 2 - COMPUTE FIN: " << comp_est_cycles << " ciclos de hardware totales.\n\n";

            std::stringstream summary;
            summary.imbue(localeCR); // Formato RAE al resumen
            
            summary << "======================================================\n"
                    << "Funcion:        " << t.name_print << "\n"
                    << "Parametros:     Samples=" << sample_count << " | Step=" << std::fixed << std::setprecision(5) << sweep_step << "\n"
                    << "Tiempos Transaccion 1 (LUT LOAD - Solo HW):\n"
                    << "  - Latencia Total:  " << std::fixed << std::setprecision(2) << load_duration_ns << " ns (" << (int)load_est_cycles << " ciclos)\n"
                    << "Tiempos Transaccion 2 (COMPUTE - Solo HW):\n"
                    << "  - Latencia Total:  " << std::fixed << std::setprecision(2) << comp_duration_ns << " ns (" << (int)comp_est_cycles << " ciclos)\n"
                    << "  - Rendimiento:     " << std::fixed << std::setprecision(2) << throughput_msps << " MSps\n"
                    << "  - Procesamiento:   Average: " << avg_ns_per_sample << " ns/spl (" << std::setprecision(2) << avg_cycles_per_sample << " ciclos/spl)\n"
                    << "Precision (Rango [" << t.lower_th << "; " << t.upper_th << "]):\n"
                    << "  - MSE:             " << std::scientific << std::setprecision(6) << mse << "\n"
                    << "  - MAE:             " << std::fixed << std::setprecision(6) << mae << "\n"
                    << "  - MNE:             " << std::scientific << std::setprecision(6) << mne << "\n"
                    << "Validacion:     " << ((local_errors == 0) ? "[SUCCESS]" : "[FAILED] (" + std::to_string(local_errors) + " Errores)") << "\n";

            std::cout << summary.str();
            res_file << summary.str();
        }
        
        std::cout << "======================================================\n";
        std::cout << "FINAL: Errores Totales HW = " << total_errors << "\n"
                  << " - Resumen: " << filename_res << "\n"
                  << " - Datalog detallado: " << filename_log << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error Critico: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    res_file.close();
    log_file.close();
    return EXIT_SUCCESS;
}