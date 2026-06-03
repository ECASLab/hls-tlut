// softmax_tb.cpp
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "api/tlut_api.cpp"

struct SoftmaxTestCase {
  std::string name;
  int N;
  double xmin;
  double xmax;
};

struct SoftmaxMetrics {
  double host_softmax_ms = 0.0;
  double tlut_process_ms = 0.0;

  double mse_exp = 0.0;
  double rmse_exp = 0.0;
  double mae_exp = 0.0;
  double max_abs_err_exp = 0.0;

  double mse_softmax = 0.0;
  double rmse_softmax = 0.0;
  double mae_softmax = 0.0;
  double max_abs_err_softmax = 0.0;
};

static inline std::int16_t q10_from_double(double x) {
  return static_cast<std::int16_t>(std::lround(x * 1024.0));
}

static inline double q10_to_double(std::int16_t x) {
  return static_cast<double>(x) / 1024.0;
}

static SoftmaxMetrics run_softmax_test(TlutAccelerator& accel, const SoftmaxTestCase& tc) {
  SoftmaxMetrics m;

  if (tc.N <= 0) {
    return m;
  }

  std::vector<double> x(tc.N);
  std::vector<double> z(tc.N);

  std::vector<double> exp_ref(tc.N);
  std::vector<double> softmax_ref(tc.N);

  std::vector<double> tlut_exp_scalar(tc.N);
  std::vector<double> tlut_softmax(tc.N);

  for (int i = 0; i < tc.N; ++i) {
    if (tc.N == 1) {
      x[i] = tc.xmin;
    } else {
      x[i] = tc.xmin + (tc.xmax - tc.xmin) * static_cast<double>(i) / static_cast<double>(tc.N - 1);
    }
  }

  // Softmax de referencia en software
  auto t_host0 = std::chrono::high_resolution_clock::now();

  const double xmax = *std::max_element(x.begin(), x.end());
  for (int i = 0; i < tc.N; ++i) {
    z[i] = x[i] - xmax;
  }

  double sum_exp_ref = 0.0;
  for (int i = 0; i < tc.N; ++i) {
    exp_ref[i] = std::exp(z[i]);
    sum_exp_ref += exp_ref[i];
  }

  for (int i = 0; i < tc.N; ++i) {
    softmax_ref[i] = exp_ref[i] / sum_exp_ref;
  }

  auto t_host1 = std::chrono::high_resolution_clock::now();
  m.host_softmax_ms = std::chrono::duration<double, std::milli>(t_host1 - t_host0).count();

  // TLUT exp(z)
  std::int16_t* in_map = accel.get_in_map();
  for (int i = 0; i < tc.N; ++i) {
    in_map[i] = q10_from_double(z[i]);
  }

  auto t_tlut0 = std::chrono::high_resolution_clock::now();
  accel.execute_process(static_cast<std::size_t>(tc.N));
  auto t_tlut1 = std::chrono::high_resolution_clock::now();
  m.tlut_process_ms = std::chrono::duration<double, std::milli>(t_tlut1 - t_tlut0).count();

  const std::int16_t* out_map = accel.get_out_map();

  double sum_exp_hw = 0.0;
  for (int i = 0; i < tc.N; ++i) {
    tlut_exp_scalar[i] = q10_to_double(out_map[i]);
    sum_exp_hw += tlut_exp_scalar[i];
  }

  for (int i = 0; i < tc.N; ++i) {
    tlut_softmax[i] = tlut_exp_scalar[i] / sum_exp_hw;
  }

  // Error de exp
  {
    double mse = 0.0;
    double mae = 0.0;
    double maxerr = 0.0;

    for (int i = 0; i < tc.N; ++i) {
      const double err = tlut_exp_scalar[i] - exp_ref[i];
      const double abse = std::abs(err);
      mse += err * err;
      mae += abse;
      maxerr = std::max(maxerr, abse);
    }

    mse /= static_cast<double>(tc.N);
    mae /= static_cast<double>(tc.N);

    m.mse_exp = mse;
    m.rmse_exp = std::sqrt(mse);
    m.mae_exp = mae;
    m.max_abs_err_exp = maxerr;
  }

  // Error de softmax
  {
    double mse = 0.0;
    double mae = 0.0;
    double maxerr = 0.0;

    for (int i = 0; i < tc.N; ++i) {
      const double err = tlut_softmax[i] - softmax_ref[i];
      const double abse = std::abs(err);
      mse += err * err;
      mae += abse;
      maxerr = std::max(maxerr, abse);
    }

    mse /= static_cast<double>(tc.N);
    mae /= static_cast<double>(tc.N);

    m.mse_softmax = mse;
    m.rmse_softmax = std::sqrt(mse);
    m.mae_softmax = mae;
    m.max_abs_err_softmax = maxerr;
  }

  return m;
}

int main(int argc, char** argv) {
  try {
    std::string binaryFile = "../HW/package.hw/kernels.xclbin";
    if (argc > 1) {
      binaryFile = argv[1];
    }

    SoftmaxTestCase tc{
      "softmax_via_tlut_exp",
      128,
      -10.0,
      10.0
    };

    TlutHardwareConfig hw_cfg;
    hw_cfg.enable_profiling = true;
    hw_cfg.max_samples = std::max<std::size_t>(hw_cfg.max_samples, static_cast<std::size_t>(tc.N));

    auto tprog0 = std::chrono::high_resolution_clock::now();
    TlutAccelerator accel(binaryFile, hw_cfg, 0);
    accel.load("exp");
    auto tprog1 = std::chrono::high_resolution_clock::now();

    const double init_time_ms = std::chrono::duration<double, std::milli>(tprog1 - tprog0).count();

    SoftmaxMetrics m = run_softmax_test(accel, tc);

    std::cout << std::fixed << std::setprecision(9);
    std::cout << "API Initialization Time = " << init_time_ms << " ms\n";
    std::cout << "Test Name = " << tc.name << "\n";
    std::cout << "N = " << tc.N << "\n";
    std::cout << "xmin = " << tc.xmin << " xmax = " << tc.xmax << "\n\n";

    std::cout << "TIMING\n";
    std::cout << "host_softmax_ms = " << m.host_softmax_ms << "\n";
    std::cout << "tlut_total_process_ms = " << m.tlut_process_ms << "\n\n";

    std::cout << "EXP_ERRORS (Comparación de HW vs C++ exp())\n";
    std::cout << "mse_exp = " << m.mse_exp << "\n";
    std::cout << "rmse_exp = " << m.rmse_exp << "\n";
    std::cout << "mae_exp = " << m.mae_exp << "\n";
    std::cout << "max_abs_err_exp = " << m.max_abs_err_exp << "\n\n";

    std::cout << "SOFTMAX_ERRORS (Probabilidad Final)\n";
    std::cout << "mse_softmax = " << m.mse_softmax << "\n";
    std::cout << "rmse_softmax = " << m.rmse_softmax << "\n";
    std::cout << "mae_softmax = " << m.mae_softmax << "\n";
    std::cout << "max_abs_err_softmax = " << m.max_abs_err_softmax << "\n";

    return 0;
  } catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << "\n";
    return 1;
  }
}