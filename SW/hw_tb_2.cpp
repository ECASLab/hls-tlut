// hw_tb.cpp
/*
 * ----------------------------------------------------------------------------
 * Copyright (c) 2026 Sergio Porras Escobar <sporras29@estudiantec.cr>
 * ----------------------------------------------------------------------------
 * Testbench para validación de Latencia y Throughput (MSps)
 * Función: SIGMOID
 * Lotes (N): 10, 100, 1000, 10000, 100000, 1000000, 10000000
 * Formato internacional estándar (separador de miles con coma, decimal con punto).
 * ----------------------------------------------------------------------------
 */

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "api/tlut_api.cpp"

// Locale para imprimir con coma separadora de miles y punto decimal
struct StandardNumberFormat : std::numpunct<char> {
    char do_decimal_point() const override { return '.'; }
    char do_thousands_sep() const override { return ','; }
    std::string do_grouping() const override { return "\3"; }
};

static constexpr double Q10_SCALE = 1024.0; // Q6.10 => 2^10

static inline std::int16_t q10_from_double(double x) {
    return static_cast<std::int16_t>(std::lround(x * Q10_SCALE));
}

double g_sigmoid(double x)  { return 1.0 / (1.0 + std::exp(-x)); }

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
    // Configurar el entorno para imprimir con formato estándar (miles y decimales)
    std::locale standardLocale(std::locale::classic(), new StandardNumberFormat);
    std::cout.imbue(standardLocale);

    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <xclbin>\n";
        return EXIT_FAILURE;
    }

    const int PROFILING_ITERATIONS = 50;
    const bool SWEEP_MODE_FIXED = true;
    const double SWEEP_FIXED_MIN = -8.0;
    const double SWEEP_FIXED_MAX = 8.0;

    // Solo se evalúa la función Sigmoid
    std::vector<TestCase> tests = {
        {"SIGMOID",  "sigmoid", -6.0, 6.0, g_sigmoid}
    };

    // Vector de lotes de muestras hasta 10 Millones
    std::vector<std::size_t> batch_sizes = {10, 100, 1000, 10000, 100000, 1000000, 10000000};

    try {
        TlutHardwareConfig hw_cfg;
        hw_cfg.enable_profiling = true;
        hw_cfg.max_samples = 10000000; // Ajustado para soportar los 10 Millones

        TlutAccelerator accel(argv[1], hw_cfg, 0);

        // Warm-up inicial
        accel.load(tests.front().folder_name);
        accel.execute_process(256);

        std::cout << std::left << std::setw(15) << "Función" 
                  << std::right << std::setw(15) << "Muestras (N)" 
                  << std::setw(25) << "Latencia Cómputo (ns)" 
                  << std::setw(25) << "Throughput (MSps)\n";
        std::cout << std::string(80, '-') << "\n";

        for (const auto& t : tests) {
            accel.load(t.folder_name);

            for (std::size_t num_samples : batch_sizes) {
                const double sweep_start = SWEEP_MODE_FIXED ? SWEEP_FIXED_MIN : t.lower_th;
                const double sweep_end   = SWEEP_MODE_FIXED ? SWEEP_FIXED_MAX : t.upper_th;

                std::vector<double> x_real;
                std::vector<std::int16_t> input_q = build_fixed_sweep_input(
                    sweep_start, sweep_end, num_samples, x_real
                );

                auto* in_map = accel.get_in_map();
                std::copy(input_q.begin(), input_q.end(), in_map);

                // Medición pura de tiempo
                const double comp_duration_ns = profile_compute(accel, num_samples, PROFILING_ITERATIONS);
                
                // Cálculo de MSps (Millones de Muestras por Segundo)
                const double throughput_msps = (static_cast<double>(num_samples) / comp_duration_ns) * 1000.0;

                std::cout << std::left << std::setw(15) << t.name_print
                          << std::right << std::setw(15) << num_samples
                          << std::fixed << std::setprecision(2) << std::setw(25) << comp_duration_ns
                          << std::fixed << std::setprecision(4) << std::setw(25) << throughput_msps << "\n";
            }
            std::cout << std::string(80, '-') << "\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error Crítico: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}