// hw_tb.cpp
/*
 * ----------------------------------------------------------------------------
 * Copyright (c) 2026 Sergio Porras Escobar <sporras29@estudiantec.cr>
 * ----------------------------------------------------------------------------
 */

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "api/tlut_api.cpp"

// ----------------------------------------------------------------------------
// Reglas de Formato (Costa Rica / RAE)
// Decimales con coma (,), sin separador de miles.
// ----------------------------------------------------------------------------
struct FormatoCR : std::numpunct<char> {
    char do_decimal_point() const override { return ','; }
    std::string do_grouping() const override { return ""; }
};

static constexpr double Q10_SCALE = 1024.0; // Q6.10 => 2^10

static inline std::int16_t q10_from_double(double x) {
    return static_cast<std::int16_t>(std::lround(x * Q10_SCALE));
}

static inline double q10_to_double(std::int16_t x) {
    return static_cast<double>(x) / Q10_SCALE;
}

double g_sigmoid(double x)  { return 1.0 / (1.0 + std::exp(-x)); }
double g_tanh(double x)     { return std::tanh(x); }
double g_softsign(double x) { return x / (1.0 + std::abs(x)); }
double g_erf(double x)      { return std::erf(x); }
double g_swish(double x)    { return x / (1.0 + std::exp(-x)); }
double g_gelu(double x)     { return 0.5 * x * (1.0 + std::erf(x / std::sqrt(2.0))); }
double g_softplus(double x)  { return std::log1p(std::exp(x)); }
double g_mish(double x)     { return x * std::tanh(std::log1p(std::exp(x))); }
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

static std::vector<std::int16_t> build_fixed_sweep_input(double start,
                                                         double end,
                                                         std::size_t num_samples,
                                                         std::vector<double>& x_real) {
    std::vector<std::int16_t> input_q(num_samples);
    x_real.clear();
    x_real.reserve(num_samples);

    const double step = (num_samples > 1)
        ? (end - start) / static_cast<double>(num_samples - 1)
        : 0.0;

    for (std::size_t i = 0; i < num_samples; ++i) {
        const double x = start + static_cast<double>(i) * step;
        x_real.push_back(x);
        input_q[i] = q10_from_double(x);
    }

    return input_q;
}

static double profile_load(TlutAccelerator& accel,
                           const std::string& func_name,
                           int iterations) {
    double best_ns = std::numeric_limits<double>::max();

    for (int i = 0; i < iterations; ++i) {
        accel.load(func_name);
        best_ns = std::min(best_ns, accel.get_last_load_duration_ns());
    }

    return best_ns;
}

static double profile_compute(TlutAccelerator& accel,
                              std::size_t samples,
                              int iterations) {
    double best_ns = std::numeric_limits<double>::max();

    for (int i = 0; i < iterations; ++i) {
        accel.execute_process(samples);
        best_ns = std::min(best_ns, accel.get_last_compute_duration_ns());
    }

    return best_ns;
}

int main(int argc, char** argv) {
    std::locale localeCR(std::locale::classic(), new FormatoCR);
    std::cout.imbue(localeCR);
    std::cerr.imbue(localeCR);

    if (argc < 2) {
    std::cerr << "Uso: " << argv[0] << " <xclbin> [num_samples]\n";
    return EXIT_FAILURE;
}

std::size_t num_samples =
    (argc >= 3) ? static_cast<std::size_t>(std::stoul(argv[2])) : 1000;

    if (num_samples == 0) {
        std::cerr << "num_samples debe ser mayor que 0.\n";
        return EXIT_FAILURE;
    }

    const std::string filename_res = "hw_results_" + std::to_string(num_samples) + ".txt";
    const std::string filename_log = "hw_interaction_" + std::to_string(num_samples) + ".log";

    std::ofstream res_file(filename_res);
    std::ofstream log_file(filename_log);

    res_file.imbue(localeCR);
    log_file.imbue(localeCR);

    if (!res_file.is_open() || !log_file.is_open()) {
        std::cerr << "No fue posible abrir archivos de salida.\n";
        return EXIT_FAILURE;
    }

    const double TOL_HARD = 0.15;
    const int PROFILING_ITERATIONS = 50;

    const bool SWEEP_MODE_FIXED = true;
    const double SWEEP_FIXED_MIN = -16.0;
    const double SWEEP_FIXED_MAX = 16.0;

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

        if (num_samples > hw_cfg.max_samples) {
            hw_cfg.max_samples = num_samples;
        }

        TlutAccelerator accel(argv[1], hw_cfg, 0);

        std::cout << "[INFO] Realizando warm-up del bus PCIe y del driver XRT...\n";
        try {
            accel.load(tests.front().folder_name);
            auto* in_map = accel.get_in_map();
            const std::size_t warmup_samples = std::min<std::size_t>(256, hw_cfg.max_samples);
            for (std::size_t i = 0; i < warmup_samples; ++i) {
                in_map[i] = 0;
            }
            accel.execute_process(warmup_samples);
        } catch (const std::exception& e) {
            std::cerr << "[WARNING] Excepcion durante warm-up: " << e.what() << '\n';
        }
        std::cout << "[INFO] Warm-up completado.\n\n";

        int total_errors = 0;

        for (const auto& t : tests) {
            const double sweep_start = SWEEP_MODE_FIXED ? SWEEP_FIXED_MIN : t.lower_th;
            const double sweep_end   = SWEEP_MODE_FIXED ? SWEEP_FIXED_MAX : t.upper_th;

            std::vector<double> x_real;
            std::vector<std::int16_t> input_q = build_fixed_sweep_input(
                sweep_start, sweep_end, num_samples, x_real
            );

            // Carga de LUTs.
            const double load_duration_ns = profile_load(accel, t.folder_name, PROFILING_ITERATIONS);
            const double load_est_cycles = load_duration_ns * (hw_cfg.fpga_freq_mhz / 1000.0);

            // Escritura directa al buffer mapeado host->FPGA.
            auto* in_map = accel.get_in_map();
            std::copy(input_q.begin(), input_q.end(), in_map);

            // Compute profiling.
            const double comp_duration_ns = profile_compute(accel, num_samples, PROFILING_ITERATIONS);
            const double comp_est_cycles = comp_duration_ns * (hw_cfg.fpga_freq_mhz / 1000.0);

            const auto* out_map = accel.get_out_map();

            std::vector<double> y_hw(num_samples);
            for (std::size_t i = 0; i < num_samples; ++i) {
                y_hw[i] = q10_to_double(out_map[i]);
            }

            double mse = 0.0;
            double mne = 0.0;
            double mae = 0.0;
            int local_errors = 0;
            int valid_samples = 0;

            std::stringstream error_details;
            error_details.imbue(localeCR);

            log_file << "======================================================\n";
            log_file << "[HW_LOG] INICIANDO TRANSACCION: " << t.name_print << '\n';
            log_file << "[HW_LOG] FASE 1 - LUT LOAD (Puro HW): " << load_est_cycles
                     << " ciclos (" << load_duration_ns << " ns)\n";
            log_file << "[HW_LOG] FASE 2 - COMPUTE INICIO: Procesando " << num_samples << " datos...\n";

            for (std::size_t j = 0; j < num_samples; ++j) {
                const double x_val = x_real[j];
                const double y_val_hw = y_hw[j];
                const double y_ideal = t.golden(x_val);

                // Barrido fijo para timing, pero métricas solo dentro del umbral útil.
                if (x_val < t.lower_th || x_val > t.upper_th) {
                    continue;
                }

                const double diff = std::abs(y_val_hw - y_ideal);

                mse += diff * diff;
                mae += diff;
                if (y_ideal != 0.0) {
                    mne += diff / std::abs(y_ideal);
                } else {
                    mne += diff;
                }

                ++valid_samples;

                if (diff > TOL_HARD) {
                    ++local_errors;
                    if (local_errors <= 10) {
                        error_details << "  -> Error #" << local_errors
                                      << " | X=" << std::fixed << std::setprecision(5) << x_val
                                      << " | Y_HW=" << y_val_hw
                                      << " | Y_IDEAL=" << y_ideal
                                      << " | DIFF=" << diff << "\n";
                    }
                }
            }

            double rmse = 0.0;
            if (valid_samples > 0) {
                mse /= static_cast<double>(valid_samples);
                mae /= static_cast<double>(valid_samples);
                mne /= static_cast<double>(valid_samples);
                rmse = std::sqrt(mse);
            }

            total_errors += local_errors;

            if (local_errors > 0) {
                log_file << "[HW_LOG] ADVERTENCIA: Se detectaron " << local_errors
                         << " muestras fuera del limite de tolerancia (TOL_HARD).\n";
                log_file << "[HW_LOG] Detalles de las primeras "
                         << std::min(local_errors, 10)
                         << " desviaciones detectadas:\n";
                log_file << error_details.str();
            } else {
                log_file << "[HW_LOG] ESTADO: Senal integra. Precision validada sin exceder tolerancia.\n";
            }

            log_file << "[HW_LOG] FASE 2 - COMPUTE FIN: " << comp_est_cycles
                     << " ciclos de hardware totales.\n\n";

            const double throughput_msps = (static_cast<double>(num_samples) / comp_duration_ns) * 1000.0;
            const double avg_ns_per_sample = comp_duration_ns / static_cast<double>(num_samples);
            const double avg_cycles_per_sample = comp_est_cycles / static_cast<double>(num_samples);

            std::stringstream summary;
            summary.imbue(localeCR);

            summary << "======================================================\n"
                    << "Funcion:        " << t.name_print << "\n"
                    << "Parametros:     Samples=" << num_samples
                    << " | Step=" << std::fixed << std::setprecision(5)
                    << ((num_samples > 1) ? ((sweep_end - sweep_start) / static_cast<double>(num_samples - 1)) : 0.0)
                    << "\n"
                    << "Tiempos Transaccion 1 (LUT LOAD - Solo HW):\n"
                    << "  - Latencia Total:  " << std::fixed << std::setprecision(2)
                    << load_duration_ns << " ns (" << load_est_cycles << " ciclos)\n"
                    << "Tiempos Transaccion 2 (COMPUTE - Solo HW):\n"
                    << "  - Latencia Total:  " << std::fixed << std::setprecision(2)
                    << comp_duration_ns << " ns (" << comp_est_cycles << " ciclos)\n"
                    << "  - Rendimiento:     " << std::fixed << std::setprecision(2)
                    << throughput_msps << " MSps\n"
                    << "  - Procesamiento:   Average: " << std::fixed << std::setprecision(2)
                    << avg_ns_per_sample << " ns/spl (" << avg_cycles_per_sample << " ciclos/spl)\n"
                    << "Precision (Rango [" << t.lower_th << "; " << t.upper_th << "]):\n"
                    << "  - MSE:             " << std::scientific << std::setprecision(6) << mse << "\n"
                    << "  - RMSE:            " << std::scientific << std::setprecision(6) << rmse << "\n"
                    << "  - MAE:             " << std::fixed << std::setprecision(6) << mae << "\n"
                    << "  - MNE:             " << std::scientific << std::setprecision(6) << mne << "\n"
                    << "Validacion:     "
                    << ((local_errors == 0)
                        ? "[SUCCESS]"
                        : "[FAILED] (" + std::to_string(local_errors) + " Errores)") << "\n";

            std::cout << summary.str();
            res_file << summary.str();
        }

        std::cout << "======================================================\n";
        std::cout << "FINAL: Errores Totales HW = " << total_errors << "\n"
                  << " - Resumen: " << filename_res << "\n"
                  << " - Datalog detallado: " << filename_log << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error Critico: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}