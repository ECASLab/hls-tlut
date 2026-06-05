// host_tb.cpp
/*
 * ----------------------------------------------------------------------------
 * Simplified Testbench for T-LUT Accelerator (tanh and exp)
 * ----------------------------------------------------------------------------
 */

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "api/tlut_api.cpp"

// Q6.10 Scaling
static constexpr double Q10_SCALE = 1024.0;

static inline std::int16_t to_q10(double x) {
    return static_cast<std::int16_t>(std::lround(x * Q10_SCALE));
}

static inline double from_q10(std::int16_t x) {
    return static_cast<double>(x) / Q10_SCALE;
}

// Golden Reference Functions
double g_tanh(double x) { return std::tanh(x); }
double g_exp(double x)  { return std::exp(x); }

struct TestInfo {
    std::string name;
    double (*golden)(double);
};

/**
 * @brief Executes a sweep test for a specific function and saves detailed results.
 */
void execute_test(TlutAccelerator& accel, const TestInfo& info, 
                  std::size_t num_samples, double x_min, double x_max) {
    
    std::vector<std::int16_t> in_q(num_samples);
    std::vector<double> x_hosts(num_samples);
    
    // Correct step calculation: range / samples (e.g., 20/100 = 0.2)
    const double step = (num_samples > 0) ? (x_max - x_min) / static_cast<double>(num_samples) : 0.0;
    
    for (std::size_t i = 0; i < num_samples; ++i) {
        x_hosts[i] = x_min + static_cast<double>(i) * step;
        in_q[i] = to_q10(x_hosts[i]);
    }

    // Load LUT and execute
    accel.load(info.name);
    std::copy(in_q.begin(), in_q.end(), accel.get_in_map());
    accel.execute_process(num_samples);
    
    const std::int16_t* out_map = accel.get_out_map();

    // Analysis and reporting
    std::string filename = "hw_results_" + info.name + ".txt";
    std::ofstream out_file(filename);
    if (!out_file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    // Structured Header with 10 decimal places for Q6.10 precision
    out_file << std::left << std::setw(8) << "Index" << " | "
             << std::setw(18) << "X_Host" << " | "
             << std::setw(18) << "X_HW (Q6.10)" << " | "
             << std::setw(18) << "Y_HW" << " | "
             << std::setw(18) << "Y_Ideal" << " | "
             << std::setw(18) << "Diff" << "\n";
    out_file << std::string(110, '-') << "\n";
    out_file << std::fixed << std::setprecision(10);

    double mse = 0.0;
    double max_err = 0.0;
    int failures = 0;
    const double TOLERANCE = 0.1;

    for (std::size_t i = 0; i < num_samples; ++i) {
        double x_hw    = from_q10(in_q[i]);
        double y_hw    = from_q10(out_map[i]);
        double y_ideal = info.golden(x_hosts[i]);
        double diff = std::abs(y_hw - y_ideal);

        out_file << std::left << std::setw(8) << i << " | "
                 << std::setw(18) << x_hosts[i] << " | "
                 << std::setw(18) << x_hw << " | "
                 << std::setw(18) << y_hw << " | "
                 << std::setw(18) << y_ideal << " | "
                 << std::setw(18) << diff << "\n";

        mse += (diff * diff);
        if (diff > max_err) max_err = diff;
        if (diff > TOLERANCE) failures++;
    }

    mse /= static_cast<double>(num_samples);

    // Console output summary
    std::cout << "Function: " << info.name << " | Samples: " << num_samples << "\n";
    std::cout << "  Range:   [" << x_min << ", " << x_max << "]\n";
    std::cout << "  MSE:     " << std::scientific << std::setprecision(4) << mse << "\n";
    std::cout << "  Max Err: " << std::fixed << std::setprecision(4) << max_err << "\n";
    std::cout << "  Status:  " << (failures == 0 ? "[PASSED]" : "[FAILED] (" + std::to_string(failures) + " samples > 0.1)") << "\n";
    std::cout << "------------------------------------------\n";
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <xclbin> [samples] [xmin] [xmax]\n";
        return EXIT_FAILURE;
    }

    const std::string xclbin_path = argv[1];
    const std::size_t samples = (argc >= 3) ? static_cast<std::size_t>(std::stoul(argv[2])) : 1000;
    const double xmin = (argc >= 4) ? std::stod(argv[3]) : -10.0;
    const double xmax = (argc >= 5) ? std::stod(argv[4]) : 10.0;

    try {
        TlutHardwareConfig hw_cfg;
        hw_cfg.max_samples = samples;

        TlutAccelerator accel(xclbin_path, hw_cfg);

        std::vector<TestInfo> tests = {
            {"tanh", g_tanh},
            {"exp",  g_exp}
        };

        std::cout << "[INFO] Initializing T-LUT host_tb...\n";
        for (const auto& t : tests) {
            execute_test(accel, t, samples, xmin, xmax);
        }

    } catch (const std::exception& e) {
        std::cerr << "Critical Error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}