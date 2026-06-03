#include <cstdint>
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <cmath>
#include <functional>
#include <numeric>
#include <algorithm>

#include "api/tlut_api.cpp"

// ============================================================================
// ESTRUCTURAS ADAPTADAS A T-LUT
// ============================================================================

struct TestCase {
  std::string name;
  std::string tlut_folder; // Reemplaza a RegionPoly y L (Ej: "gelu", "exp")
  int N;
  double xmin;
  double xmax;
  std::function<double(double)> ref_fn;
};

struct Metrics {
  double cfg_write_ms = 0.0; // Tiempo en cargar las tablas (accel.load)
  double h2d_ms = 0.0;       // (Asimilado en execute_process)
  double kernel_ms = 0.0;    // Tiempo puro de procesamiento PCIe + HW
  double d2h_ms = 0.0;       // (Asimilado en execute_process)
  double total_ms = 0.0;

  double mse = 0.0;
  double rmse = 0.0;
  double mae = 0.0;
  double max_abs_err = 0.0;
};

struct SummaryRow {
  std::string name;
  int N = 0;
  double avg_cfg_write_ms = 0.0;
  double avg_h2d_ms = 0.0;
  double avg_kernel_ms = 0.0;
  double avg_d2h_ms = 0.0;
  double avg_total_ms = 0.0;
  double throughput_samples_per_sec = 0.0;
  double cfg_over_kernel_pct = 0.0;

  double avg_kernel_us = 0.0;
  double ns_per_sample = 0.0;
  double cycles_at_250MHz = 0.0;

  double avg_mse = 0.0;
  double avg_rmse = 0.0;
  double avg_mae = 0.0;
  double avg_max_abs_err = 0.0;
};

// ============================================================================
// FUNCIONES DE REFERENCIA MATEMÁTICA (Mantenidas del original)
// ============================================================================

static double ref_gelu(double x) {
  const double c = std::sqrt(2.0 / M_PI);
  return 0.5 * x * (1.0 + std::tanh(c * (x + 0.044715 * x * x * x)));
}

static double ref_tanh(double x) {
  return std::tanh(x);
}

static double ref_sigmoid(double x) {
  return 1.0 / (1.0 + std::exp(-x));
}

static double ref_swish(double x) {
  return x / (1.0 + std::exp(-x));
}

static double ref_exp_fn(double x) {
  return std::exp(x);
}

// ============================================================================
// MOTOR DE EJECUCIÓN (Motor t-LUT)
// ============================================================================

static Metrics run_test(
  TlutAccelerator& accel,
  std::vector<double>& x_scalar,
  std::vector<double>& y_scalar,
  const TestCase& tc,
  bool compute_error
) {
  Metrics m;
  const double scale_factor = 1024.0; // Q6.10

  auto t_total_0 = std::chrono::high_resolution_clock::now();

  // FASE 1: Generar el barrido (Line-space)
  for (int i = 0; i < tc.N; ++i) {
    x_scalar[i] = tc.xmin + (tc.xmax - tc.xmin) * (double)i / (double)(tc.N - 1);
  }

  // FASE 2: Configuración (Carga de tablas)
  auto t_cfg_0 = std::chrono::high_resolution_clock::now();
  accel.load(tc.tlut_folder);
  auto t_cfg_1 = std::chrono::high_resolution_clock::now();

  // FASE 3: Empaquetar y transferir a punto fijo
  uint16_t* in_map = accel.get_in_map();
  for (int i = 0; i < tc.N; ++i) {
    int16_t fixed_val = static_cast<int16_t>(x_scalar[i] * scale_factor);
    in_map[i] = static_cast<uint16_t>(fixed_val);
  }

  // FASE 4: Ejecución en Hardware (Incluye Transferencia PCIe)
  auto t_k_0 = std::chrono::high_resolution_clock::now();
  accel.execute_process(tc.N);
  auto t_k_1 = std::chrono::high_resolution_clock::now();

  // FASE 5: Leer resultados de hardware
  const uint16_t* out_map = accel.get_out_map();
  for (int i = 0; i < tc.N; ++i) {
    int16_t out_fixed = static_cast<int16_t>(out_map[i]);
    y_scalar[i] = static_cast<double>(out_fixed) / scale_factor;
  }

  // FASE 6: Calcular Errores
  if (compute_error) {
    double mse = 0.0;
    double mae = 0.0;
    double max_abs_err = 0.0;

    for (int i = 0; i < tc.N; ++i) {
      double x = x_scalar[i];
      double y_hw = y_scalar[i];
      double y_ref = tc.ref_fn(x);
      double err = y_hw - y_ref;
      double abs_err = std::abs(err);

      mse += err * err;
      mae += abs_err;
      if (abs_err > max_abs_err) max_abs_err = abs_err;
    }

    mse /= (double)tc.N;
    mae /= (double)tc.N;

    m.mse = mse;
    m.rmse = std::sqrt(mse);
    m.mae = mae;
    m.max_abs_err = max_abs_err;
  }

  auto t_total_1 = std::chrono::high_resolution_clock::now();

  // Registrar Tiempos
  m.cfg_write_ms = std::chrono::duration<double, std::milli>(t_cfg_1 - t_cfg_0).count();
  // Al usar Zero-Copy en execute_process(), la métrica de Kernel abarca todo el viaje (H2D+Run+D2H)
  m.kernel_ms = std::chrono::duration<double, std::milli>(t_k_1 - t_k_0).count(); 
  m.total_ms = std::chrono::duration<double, std::milli>(t_total_1 - t_total_0).count();

  return m;
}

static double avg(const std::vector<double>& v) {
  if (v.empty()) return 0.0;
  return std::accumulate(v.begin(), v.end(), 0.0) / (double)v.size();
}

// ============================================================================
// MAIN SCRIPT
// ============================================================================

int main(int argc, char** argv) {
  try {
    std::string binaryFile = "../HW/package.hw/kernels.xclbin";
    std::string mode = "all";

    for (int i = 1; i < argc; ++i) {
      std::string arg = argv[i];

      if (arg == "--mode" && i + 1 < argc) {
        mode = argv[++i];
      } else if (arg == "-all") {
        mode = "all";
      } else if (arg == "-timing") {
        mode = "timing";
      } else if (arg == "-functional") {
        mode = "functional";
      } else {
        binaryFile = arg;
      }
    }

    const bool do_functional = (mode == "functional" || mode == "all");
    const bool do_timing     = (mode == "timing" || mode == "all");

    const int maxN = 10000;
    const int warmup_runs = 1;
    const int measured_runs = 5;

    // Equivalencia de Casos de Prueba (Se mapean a la carpeta TLUT)
    std::vector<TestCase> tests = {
      { "gelu_N10000", "gelu", 10000, -8.0, 8.0, ref_gelu },
      { "tanh_N4096",  "tanh",  4096, -4.0, 4.0, ref_tanh },
      { "sigmoid_N4096", "sigmoid", 4096, -8.0, 8.0, ref_sigmoid },
      { "swish_N4096", "swish", 4096, -8.0, 8.0, ref_swish },
      
      { "gelu_N1024", "gelu", 1024, -8.0, 8.0, ref_gelu },
      { "tanh_N1024", "tanh", 1024, -4.0, 4.0, ref_tanh },
      { "sigmoid_N1024", "sigmoid", 1024, -8.0, 8.0, ref_sigmoid },
      { "swish_N1024", "swish", 1024, -8.0, 8.0, ref_swish },
      
      { "exp_RangoLargo_N1024", "exp", 1024, -4.0, 4.0, ref_exp_fn },
      { "exp_RangoLargo_N4096", "exp", 4096, -4.0, 4.0, ref_exp_fn },
      { "exp_Softmax_N1024", "exp", 1024, -1.0, 1.0, ref_exp_fn },
      { "exp_Softmax_N4096", "exp", 4096, -1.0, 1.0, ref_exp_fn },
    };

    auto tprog0 = std::chrono::high_resolution_clock::now();
    
    // Configuración Inicial e Instanciación de tu API
    TlutHardwareConfig hw_cfg;
    hw_cfg.max_samples = maxN;
    TlutAccelerator accel(binaryFile, hw_cfg, 0);

    auto tprog1 = std::chrono::high_resolution_clock::now();
    double xclbin_program_time_ms = std::chrono::duration<double, std::milli>(tprog1 - tprog0).count();

    std::vector<double> x_scalar(maxN);
    std::vector<double> y_scalar(maxN);
    std::vector<SummaryRow> summary;

    std::cout << std::fixed << std::setprecision(9);
    std::cout << "xclbin_program_time_ms=" << xclbin_program_time_ms << "\n";
    std::cout << "mode=" << mode << "\n";
    std::cout << "warmup_runs=" << warmup_runs << "\n";
    std::cout << "measured_runs=" << measured_runs << "\n\n";

    for (const auto& tc : tests) {
      // Warmups
      for (int i = 0; i < warmup_runs; ++i) {
        (void)run_test(accel, x_scalar, y_scalar, tc, do_functional);
      }

      std::vector<double> cfg_list, h2d_list, kernel_list, d2h_list, total_list;
      std::vector<double> mse_list, rmse_list, mae_list, maxerr_list;

      for (int i = 0; i < measured_runs; ++i) {
        Metrics m = run_test(accel, x_scalar, y_scalar, tc, do_functional);

        if (do_timing) {
          cfg_list.push_back(m.cfg_write_ms);
          h2d_list.push_back(m.h2d_ms);
          kernel_list.push_back(m.kernel_ms);
          d2h_list.push_back(m.d2h_ms);
          total_list.push_back(m.total_ms);
        }

        if (do_functional) {
          mse_list.push_back(m.mse);
          rmse_list.push_back(m.rmse);
          mae_list.push_back(m.mae);
          maxerr_list.push_back(m.max_abs_err);
        }
      }

      SummaryRow row;
      row.name = tc.name;
      row.N = tc.N;

      if (do_timing) {
        row.avg_cfg_write_ms = avg(cfg_list);
        row.avg_h2d_ms = avg(h2d_list);
        row.avg_kernel_ms = avg(kernel_list);
        row.avg_d2h_ms = avg(d2h_list);
        row.avg_total_ms = avg(total_list);

        row.throughput_samples_per_sec =
          (row.avg_kernel_ms > 0.0) ? ((double)tc.N / (row.avg_kernel_ms * 1e-3)) : 0.0;

        row.cfg_over_kernel_pct =
          (row.avg_kernel_ms > 0.0) ? (100.0 * row.avg_cfg_write_ms / row.avg_kernel_ms) : 0.0;

        row.avg_kernel_us = row.avg_kernel_ms * 1000.0;
        row.ns_per_sample =
          (row.N > 0) ? (row.avg_kernel_ms * 1e6 / (double)row.N) : 0.0;

        row.cycles_at_250MHz =
          row.avg_kernel_ms * 1e-3 * 250e6; // Asume 250 MHz como en el original
      }

      if (do_functional) {
        row.avg_mse = avg(mse_list);
        row.avg_rmse = avg(rmse_list);
        row.avg_mae = avg(mae_list);
        row.avg_max_abs_err = avg(maxerr_list);
      }

      summary.push_back(row);

      // Impresión en tiempo real (Idéntica al original)
      std::cout << "========================================\n";
      std::cout << "test_name=" << tc.name << "\n";
      std::cout << "N=" << tc.N << " xmin=" << tc.xmin << " xmax=" << tc.xmax << "\n";

      if (do_timing) {
        std::cout << "avg_cfg_write_ms=" << row.avg_cfg_write_ms << "\n";
        std::cout << "avg_kernel_ms=" << row.avg_kernel_ms << "\n";
        std::cout << "avg_kernel_us=" << row.avg_kernel_us << "\n";
        std::cout << "ns_per_sample=" << row.ns_per_sample << "\n";
        std::cout << "cycles_at_250MHz=" << row.cycles_at_250MHz << "\n";
        std::cout << "avg_total_ms=" << row.avg_total_ms << "\n";
        std::cout << "throughput_samples_per_sec=" << row.throughput_samples_per_sec << "\n";
        std::cout << "cfg_over_kernel_pct=" << row.cfg_over_kernel_pct << "\n";
      }

      if (do_functional) {
        std::cout << "avg_mse=" << row.avg_mse << "\n";
        std::cout << "avg_rmse=" << row.avg_rmse << "\n";
        std::cout << "avg_mae=" << row.avg_mae << "\n";
        std::cout << "avg_max_abs_err=" << row.avg_max_abs_err << "\n";
      }
      std::cout << "\n";
    }

    // ========================================================================
    // TABLA DE RESUMEN FINAL (Estructura Intacta)
    // ========================================================================
    std::cout << "========================================\n";
    std::cout << "FINAL_SUMMARY\n";

    if (do_timing) {

      const int W_FUNC   = 30;
      const int W_INT    = 8;
      const int W_FLOAT  = 16;
      const int W_FLOATL = 20;

      std::cout << std::left
                << std::setw(W_FUNC)   << "Function"
                << std::right
                << std::setw(W_INT)    << "N"
                << std::setw(W_FLOAT)  << "CfgWrite_ms"
                << std::setw(W_FLOAT)  << "Kernel_ms"
                << std::setw(W_FLOAT)  << "Kernel_us"
                << std::setw(W_FLOAT)  << "NsPerSample"
                << std::setw(W_FLOATL) << "Cycles250MHz"
                << std::setw(W_FLOATL) << "Throughput_sps"
                << std::setw(W_FLOAT)  << "CfgOverKernel_pct";

      if (do_functional) {
        std::cout << std::setw(W_FLOAT) << "MSE"
                  << std::setw(W_FLOAT) << "RMSE"
                  << std::setw(W_FLOAT) << "MAE"
                  << std::setw(W_FLOAT) << "MaxAbsErr";
      }
      std::cout << "\n";

      for (const auto& row : summary) {
        std::cout << std::left
                  << std::setw(W_FUNC)   << row.name
                  << std::right
                  << std::setw(W_INT)    << row.N
                  << std::setw(W_FLOAT)  << row.avg_cfg_write_ms
                  << std::setw(W_FLOAT)  << row.avg_kernel_ms
                  << std::setw(W_FLOAT)  << row.avg_kernel_us
                  << std::setw(W_FLOAT)  << row.ns_per_sample
                  << std::setw(W_FLOATL) << row.cycles_at_250MHz
                  << std::setw(W_FLOATL) << row.throughput_samples_per_sec
                  << std::setw(W_FLOAT)  << row.cfg_over_kernel_pct;

        if (do_functional) {
          std::cout << std::setw(W_FLOAT) << row.avg_mse
                    << std::setw(W_FLOAT) << row.avg_rmse
                    << std::setw(W_FLOAT) << row.avg_mae
                    << std::setw(W_FLOAT) << row.avg_max_abs_err;
        }
        std::cout << "\n";
      }
    }
    return 0;
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << "\n";
    return 1;
  }
}