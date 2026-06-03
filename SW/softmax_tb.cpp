#include <cstdint>
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>

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

static SoftmaxMetrics run_softmax_test(TlutAccelerator& accel, const SoftmaxTestCase& tc) {
  SoftmaxMetrics m;

  std::vector<double> x(tc.N);
  std::vector<double> z(tc.N);

  std::vector<double> exp_ref(tc.N);
  std::vector<double> softmax_ref(tc.N);

  std::vector<double> tlut_exp_scalar(tc.N);
  std::vector<double> tlut_softmax(tc.N);

  // Generar datos de prueba
  for (int i = 0; i < tc.N; ++i) {
    x[i] = tc.xmin + (tc.xmax - tc.xmin) * (double)i / (double)(tc.N - 1);
  }

  // ---------------------------------
  // Host softmax completo (Software Puro)
  // ---------------------------------
  auto t_host0 = std::chrono::high_resolution_clock::now();

  const double xmax = *std::max_element(x.begin(), x.end());

  for (int i = 0; i < tc.N; ++i) {
    z[i] = x[i] - xmax; // Resta del Safe Softmax
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

  // ---------------------------------
  // Acelerador TLUT para exp(z)
  // ---------------------------------
  uint16_t* in_map = accel.get_in_map();
  
  // Convertir double a Q6.10 (Factor = 1024.0)
  for (int i = 0; i < tc.N; ++i) {
    int16_t fixed_val = static_cast<int16_t>(z[i] * 1024.0);
    in_map[i] = static_cast<uint16_t>(fixed_val);
  }

  auto t_tlut0 = std::chrono::high_resolution_clock::now();
  
  // Ejecutar HW (Transferencias PCIe + Computo)
  accel.execute_process(tc.N);
  
  auto t_tlut1 = std::chrono::high_resolution_clock::now();
  m.tlut_process_ms = std::chrono::duration<double, std::milli>(t_tlut1 - t_tlut0).count();

  const uint16_t* out_map = accel.get_out_map();

  // Recuperar resultados y convertir Q6.10 a double
  double sum_exp_hw = 0.0;
  for (int i = 0; i < tc.N; ++i) {
    int16_t fixed_out = static_cast<int16_t>(out_map[i]);
    tlut_exp_scalar[i] = static_cast<double>(fixed_out) / 1024.0;
    sum_exp_hw += tlut_exp_scalar[i];
  }

  for (int i = 0; i < tc.N; ++i) {
    tlut_softmax[i] = tlut_exp_scalar[i] / sum_exp_hw;
  }

  // ---------------------------------
  // Error de exp
  // ---------------------------------
  double mse_exp = 0.0;
  double mae_exp = 0.0;
  double maxerr_exp = 0.0;

  for (int i = 0; i < tc.N; ++i) {
    double err = tlut_exp_scalar[i] - exp_ref[i];
    double abse = std::abs(err);
    mse_exp += err * err;
    mae_exp += abse;
    if (abse > maxerr_exp) maxerr_exp = abse;
  }

  mse_exp /= (double)tc.N;
  mae_exp /= (double)tc.N;

  m.mse_exp = mse_exp;
  m.rmse_exp = std::sqrt(mse_exp);
  m.mae_exp = mae_exp;
  m.max_abs_err_exp = maxerr_exp;

  // ---------------------------------
  // Error de softmax
  // ---------------------------------
  double mse_sm = 0.0;
  double mae_sm = 0.0;
  double maxerr_sm = 0.0;

  for (int i = 0; i < tc.N; ++i) {
    double err = tlut_softmax[i] - softmax_ref[i];
    double abse = std::abs(err);
    mse_sm += err * err;
    mae_sm += abse;
    if (abse > maxerr_sm) maxerr_sm = abse;
  }

  mse_sm /= (double)tc.N;
  mae_sm /= (double)tc.N;

  m.mse_softmax = mse_sm;
  m.rmse_softmax = std::sqrt(mse_sm);
  m.mae_softmax = mae_sm;
  m.max_abs_err_softmax = maxerr_sm;

  return m;
}

int main(int argc, char** argv) {
  try {
    std::string binaryFile = "../HW/package.hw/kernels.xclbin";

    if (argc > 1) {
      binaryFile = argv[1];
    }

    // Caso de prueba para t-LUT
    SoftmaxTestCase tc = {
      "softmax_via_tlut_exp",
      128,          // N: Cantidad de números
      -10.0, 10.0   // Rango de prueba (Puedes ajustarlo)
    };

    auto tprog0 = std::chrono::high_resolution_clock::now();
    
    // Inicialización Limpia usando tu API
    TlutAccelerator accel(binaryFile);
    accel.load("exp"); // Cargamos la tabla exponencial
    
    auto tprog1 = std::chrono::high_resolution_clock::now();
    double init_time_ms = std::chrono::duration<double, std::milli>(tprog1 - tprog0).count();

    // Correr Testbench
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
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << "\n";
    return 1;
  }
}