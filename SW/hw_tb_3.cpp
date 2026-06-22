// hw_tb_exact_sweep.cpp
/*
 * ----------------------------------------------------------------------------
 * Copyright (c) 2026 Sergio Porras Escobar <sporras29@estudiantec.cr>
 * ----------------------------------------------------------------------------
 * Testbench: Barrido exacto de puntos enteros del eje X (-8 a 8)
 * Funciones: SIGMOID, TANH, SWISH, ELU, EXP, SQRT
 * Muestras: 17 puntos exactos por función
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

int main(int argc, char** argv) {
    std::locale localeCR(std::locale::classic(), new FormatoCR);
    std::cout.imbue(localeCR);

    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <xclbin>\n";
        return EXIT_FAILURE;
    }

    // Generamos los 17 puntos exactos de interés
    std::vector<double> x_puntos;
    for (int i = -8; i <= 8; ++i) {
        x_puntos.push_back(static_cast<double>(i));
    }
    const std::size_t num_samples = x_puntos.size();

    std::vector<TestCase> tests = {
        {"SIGMOID",  "sigmoid",     -6.0,   6.0, g_sigmoid},
        {"TANH",     "tanh",        -4.0,   4.0, g_tanh},
        {"SWISH",    "swish",       -6.0,   6.0, g_swish},
        {"ELU",      "elu",         -6.0,   0.0, g_elu},
        {"EXP",      "exp",         -8.0,   1.0, g_exp},
        {"SQRT",     "sqrt",         0.0,  15.875, g_sqrt}
    };

    try {
        TlutHardwareConfig hw_cfg;
        hw_cfg.enable_profiling = true;
        hw_cfg.max_samples = num_samples;

        TlutAccelerator accel(argv[1], hw_cfg, 0);

        // Warm-up inicial
        accel.load(tests.front().folder_name);
        accel.execute_process(1);

        std::cout << std::left << std::setw(15) << "Funcion" 
                  << std::setw(12) << "Punto X" 
                  << std::setw(20) << "Error Absoluto" 
                  << "Latencia Cómputo\n";
        std::cout << std::string(65, '-') << "\n";

        for (const auto& t : tests) {
            accel.load(t.folder_name);

            // Construir el vector cuantizado con los enteros exactos
            std::vector<std::int16_t> input_q(num_samples);
            for (std::size_t i = 0; i < num_samples; ++i) {
                input_q[i] = q10_from_double(x_puntos[i]);
            }

            auto* in_map = accel.get_in_map();
            std::copy(input_q.begin(), input_q.end(), in_map);

            // Procesar los 17 datos en el hardware
            accel.execute_process(num_samples);
            const double comp_duration_ns = accel.get_last_compute_duration_ns();
            
            const auto* out_map = accel.get_out_map();

            for (std::size_t i = 0; i < num_samples; ++i) {
                const double x_val = x_puntos[i];
                const int point_x = static_cast<int>(x_val);

                std::cout << std::left << std::setw(15) << t.name_print;
                
                std::string s_point = (point_x > 0 ? "+" : "") + std::to_string(point_x) + ",0";
                if (point_x == 0) s_point = "0,0";
                std::cout << std::setw(12) << s_point;

                // Si está fuera del umbral operativo de la t-LUT, se marca N/A
                if (x_val < t.lower_th || x_val > t.upper_th) {
                    std::cout << std::setw(20) << "N/A (Fuera Umbral)"
                              << std::fixed << std::setprecision(2) << comp_duration_ns << " ns\n";
                    continue;
                }

                const double y_val_hw = q10_to_double(out_map[i]);
                const double y_ideal = t.golden(x_val);
                const double error_abs = std::abs(y_val_hw - y_ideal);

                std::cout << std::fixed << std::setprecision(8) << std::setw(20) << error_abs
                          << std::fixed << std::setprecision(2) << comp_duration_ns << " ns\n";
            }
            std::cout << std::string(65, '-') << "\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error Crítico: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}