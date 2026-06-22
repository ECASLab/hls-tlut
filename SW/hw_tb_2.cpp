// hw_tb.cpp
/*
 * ----------------------------------------------------------------------------
 * Copyright (c) 2026 Sergio Porras Escobar <sporras29@estudiantec.cr>
 * ----------------------------------------------------------------------------
 * Testbench simplificado para validación de RMSE y Latencia
 * Funciones: SIGMOID, TANH, SWISH, ELU, EXP, SQRT
 * Lotes (N): 10, 100, 1000, 10000, 100000, 1000000
 * ----------------------------------------------------------------------------
 */

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <locale>
#include <limits>
#include <string>
#include <vector>

#include "api/tlut_api.cpp"

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
double g_swish(double x)    { return x / (1.0 + std::exp(-x)); }
double g_elu(double x)      { return (x < 0.0) ? (std::exp(x) - 1.0) : x; }
double g_exp(double x)      { return std::exp(x); }
double g_sqrt(double x)     { return (x >= 0.0) ? std::sqrt(x) : 0.0; }

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

    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <xclbin>\n";
        return EXIT_FAILURE;
    }

    const int PROFILING_ITERATIONS = 50;
    const bool SWEEP_MODE_FIXED = true;
    const double SWEEP_FIXED_MIN = -8.0;
    const double SWEEP_FIXED_MAX = 8.0;

    // Solo las 6 funciones solicitadas
    std::vector<TestCase> tests = {
        {"SIGMOID",  "sigmoid",     -6.0,   6.0, g_sigmoid},
        {"TANH",     "tanh",        -4.0,   4.0, g_tanh},
        {"SWISH",    "swish",       -6.0,   6.0, g_swish},
        {"ELU",      "elu",         -6.0,   0.0, g_elu},
        {"EXP",      "exp",         -8.0,   1.0, g_exp},
        {"SQRT",     "sqrt",         0.0,  15.875, g_sqrt}
    };

    // Vector de lotes de muestras a probar iterativamente
    std::vector<std::size_t> batch_sizes = {10, 100, 1000, 10000, 100000, 1000000};

    try {
        TlutHardwareConfig hw_cfg;
        hw_cfg.enable_profiling = true;
        hw_cfg.max_samples = 1000000; // Ajustado para soportar el lote máximo directamente

        TlutAccelerator accel(argv[1], hw_cfg, 0);

        // Warm-up inicial
        accel.load(tests.front().folder_name);
        accel.execute_process(256);

        std::cout << std::left << std::setw(15) << "Funcion" 
                  << std::setw(15) << "Muestras (N)" 
                  << std::setw(20) << "RMSE" 
                  << "Latencia Computo (ns)\n";
        std::cout << std::string(75, '-') << "\n";

        for (const auto& t : tests) {
            accel.load(t.folder_name); // Se carga la LUT una sola vez por función

            for (std::size_t num_samples : batch_sizes) {
                const double sweep_start = SWEEP_MODE_FIXED ? SWEEP_FIXED_MIN : t.lower_th;
                const double sweep_end   = SWEEP_MODE_FIXED ? SWEEP_FIXED_MAX : t.upper_th;

                std::vector<double> x_real;
                std::vector<std::int16_t> input_q = build_fixed_sweep_input(
                    sweep_start, sweep_end, num_samples, x_real
                );

                // Escritura al buffer
                auto* in_map = accel.get_in_map();
                std::copy(input_q.begin(), input_q.end(), in_map);

                // Profiling de cómputo
                const double comp_duration_ns = profile_compute(accel, num_samples, PROFILING_ITERATIONS);
                
                const auto* out_map = accel.get_out_map();

                // Cálculo de MSE y RMSE
                double mse = 0.0;
                int valid_samples = 0;

                for (std::size_t j = 0; j < num_samples; ++j) {
                    const double x_val = x_real[j];
                    
                    // Solo se evalúa dentro de los umbrales de precisión válidos
                    if (x_val < t.lower_th || x_val > t.upper_th) {
                        continue;
                    }

                    const double y_val_hw = q10_to_double(out_map[j]);
                    const double y_ideal = t.golden(x_val);
                    const double diff = std::abs(y_val_hw - y_ideal);

                    mse += diff * diff;
                    ++valid_samples;
                }

                if (valid_samples > 0) {
                    mse /= static_cast<double>(valid_samples);
                }

                double rmse = std::sqrt(mse);

                // Impresión de resultados
                std::cout << std::left << std::setw(15) << t.name_print
                          << std::setw(15) << num_samples
                          << std::fixed << std::setprecision(8) << std::setw(20) << rmse
                          << std::fixed << std::setprecision(2) << comp_duration_ns << "\n";
            }
            std::cout << std::string(75, '-') << "\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error Critico: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}