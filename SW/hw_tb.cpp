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

// Cabeceras de XRT (Xilinx Runtime)
#include "xrt/xrt_bo.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"

// Cabeceras de nuestro diseño (proveen data_t, axi_word_t, structs, etc)
#include "nla_core.h"
#include "tluts_B16.h"

// -------------------------------------------------------------------------
// Referencias Golden
// -------------------------------------------------------------------------
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
    std::string name;
    const int* dlut;
    const int* elut;
    const int* control;
    double (*golden)(double);
};

// -------------------------------------------------------------------------
// MAIN EXECUTION
// -------------------------------------------------------------------------
int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <ruta_al_archivo.xclbin>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    // Constantes de HW (Deben coincidir con los pragmas m_axi depth)
    const int HW_MAX_SAMPLES = 100000;
    const int DLUT_WORDS_AXI = 128;
    const int ELUT_WORDS_AXI = 512;

    const double SWEEP_STEP      = 0.1;
    const double RANGE_PADDING   = 1.0;
    const double TOL_HARD_NORMAL = 0.15;
    const double TOL_HARD_LARGE  = 0.40;
    
    double q_scale = (double)(1 << (Q_TOT_WIDTH - Q_INT_WIDTH)); 

    std::vector<TestCase> tests = {
        {"SIGMOID", SIGMOID_DLUT, SIGMOID_ELUT, SIGMOID_CONTROL, g_sigmoid},
        {"TANH", TANH_DLUT, TANH_ELUT, TANH_CONTROL, g_tanh},
        {"SOFTSIGN", SOFTSIGN_DLUT, SOFTSIGN_ELUT, SOFTSIGN_CONTROL, g_softsign},
        {"ERF", ERF_DLUT, ERF_ELUT, ERF_CONTROL, g_erf},
        {"SWISH", SWISH_DLUT, SWISH_ELUT, SWISH_CONTROL, g_swish},
        {"GELU", GELU_DLUT, GELU_ELUT, GELU_CONTROL, g_gelu},
        {"SOFTPLUS", SOFTPLUS_DLUT, SOFTPLUS_ELUT, SOFTPLUS_CONTROL, g_softplus},
        {"MISH", MISH_DLUT, MISH_ELUT, MISH_CONTROL, g_mish},
        {"ELU", ELU_DLUT, ELU_ELUT, ELU_CONTROL, g_elu},
        {"EXP", EXP_DLUT, EXP_ELUT, EXP_CONTROL, g_exp},
        {"SQRT", SQRT_DLUT, SQRT_ELUT, SQRT_CONTROL, g_sqrt},
        {"RELU", RELU_DLUT, RELU_ELUT, RELU_CONTROL, g_relu}
    };

    std::cout << "Inicializando FPGA y cargando Bitstream..." << std::endl;
    auto device = xrt::device(0);
    auto uuid = device.load_xclbin(binaryFile);
    auto kernel = xrt::kernel(device, uuid, "nla_top");

    // Creacion de Buffer Objects (BO) en la memoria DDR de la FPGA
    auto bo_in  = xrt::bo(device, HW_MAX_SAMPLES * sizeof(data_t), kernel.group_id(0));
    auto bo_out = xrt::bo(device, HW_MAX_SAMPLES * sizeof(data_t), kernel.group_id(1));
    auto bo_d   = xrt::bo(device, DLUT_WORDS_AXI * sizeof(ap_uint<128>), kernel.group_id(2));
    auto bo_e   = xrt::bo(device, ELUT_WORDS_AXI * sizeof(ap_uint<128>), kernel.group_id(3));

    // Mapeo a espacio de usuario del Host (CPU)
    data_t* in_map        = bo_in.map<data_t*>();
    data_t* out_map       = bo_out.map<data_t*>();
    ap_uint<128>* d_map   = bo_d.map<ap_uint<128>*>();
    ap_uint<128>* e_map   = bo_e.map<ap_uint<128>*>();

    std::ofstream res_file("hw_results.txt");
    res_file << "X_REAL\tX_HW\tY_HW\tY_IDEAL\tDIFF\n";

    int total_errors = 0;

    for (const auto& t : tests) {
        int upper = t.control[1];
        int lower = t.control[2];
        int active_depth = (upper - lower) * (int)q_scale + 1;
        int d_cap = (active_depth + B_SIZE - 1) / B_SIZE;

        // Limpiar memoria de los mapas
        std::memset(d_map, 0, DLUT_WORDS_AXI * sizeof(ap_uint<128>));
        std::memset(e_map, 0, ELUT_WORDS_AXI * sizeof(ap_uint<128>));

        // Empaquetado E-LUT hacia la BRAM
        for (int c = 0; c < (active_depth + E_RESHAPE_FACTOR - 1) / E_RESHAPE_FACTOR; c++) {
            ap_uint<128> word = 0;
            for (int j = 0; j < E_RESHAPE_FACTOR; j++) {
                int idx = c * E_RESHAPE_FACTOR + j;
                if (idx < active_depth) word.range(j * ELUT_WIDTH + (ELUT_WIDTH - 1), j * ELUT_WIDTH) = t.elut[idx];
            }
            e_map[c] = word;
        }

        // Empaquetado D-LUT hacia la BRAM
        for (int c = 0; c < (d_cap + D_RESHAPE_FACTOR - 1) / D_RESHAPE_FACTOR; c++) {
            ap_uint<128> word = 0;
            for (int j = 0; j < D_RESHAPE_FACTOR; j++) {
                int idx = c * D_RESHAPE_FACTOR + j;
                if (idx < d_cap) {
                    data_t val = (float)t.dlut[idx] / q_scale; 
                    word.range(j * DLUT_WIDTH + (DLUT_WIDTH - 1), j * DLUT_WIDTH) = val.range(DLUT_WIDTH - 1, 0);
                }
            }
            d_map[c] = word;
        }

        // Transferir las tablas LUT de CPU a FPGA
        bo_d.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        bo_e.sync(XCL_BO_SYNC_BO_TO_DEVICE);

        // TRANSACCIÓN 1: MODO CONFIGURACIÓN Y RECARGA
        nla_config_t config_load;
        config_load.reload_tlut = 1;
        config_load.active_depth = active_depth;
        config_load.num_samples = 0;
        
        auto run_load = kernel(bo_in, bo_out, bo_d, bo_e, config_load);
        run_load.wait();

        // Preparar estímulos para la corrida
        double sweep_min = (double)lower - RANGE_PADDING;
        double sweep_max = (double)upper + RANGE_PADDING;
        
        std::vector<double> x_original;
        int sample_count = 0;

        for (double x = sweep_min; x <= sweep_max; x += SWEEP_STEP) {
            if (sample_count < HW_MAX_SAMPLES) {
                in_map[sample_count] = (data_t)x;
                x_original.push_back(x);
                sample_count++;
            }
        }

        // Transferir estímulos al FPGA
        bo_in.sync(XCL_BO_SYNC_BO_TO_DEVICE);

        // TRANSACCIÓN 2: MODO CÓMPUTO DATAFLOW
        nla_config_t config_run;
        config_run.c_sym = t.control[0];
        config_run.upper_threshold = t.control[1];
        config_run.lower_threshold = t.control[2];
        config_run.c_upper = t.control[3];
        config_run.c_lower = t.control[4];
        config_run.use_sym = t.control[5];
        config_run.use_lin = t.control[6];
        config_run.reload_tlut = 0;
        config_run.active_depth = active_depth;
        config_run.num_samples = sample_count;

        auto run_compute = kernel(bo_in, bo_out, bo_d, bo_e, config_run);
        run_compute.wait();

        // Transferir resultados del FPGA a CPU
        bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

        // VALIDACIÓN Y ESTADÍSTICAS
        res_file << "--- Banco: " << t.name << " ---\n";
        
        double mse = 0.0;
        int local_errors = 0;

        for (int j = 0; j < sample_count; j++) {
            double x_val = x_original[j];
            double x_hw  = in_map[j].to_double();
            double y_hw  = out_map[j].to_double();
            double y_expected = t.golden(x_val);

            // Regiones de saturación teóricas
            if (x_val > (double)upper || x_val < (double)lower) y_expected = y_hw; 

            double diff = std::abs(y_hw - y_expected);
            mse += (diff * diff);

            double hard_tol = (std::abs(y_expected) > 10.0) ? TOL_HARD_LARGE : TOL_HARD_NORMAL;
            if (diff > hard_tol) local_errors++;

            res_file << std::fixed << std::setprecision(4) 
                     << x_val << "\t" << x_hw << "\t" << y_hw << "\t" << y_expected << "\t" << diff << "\n";
        }
        
        mse /= sample_count;
        total_errors += local_errors;

        // IMPRESIÓN EN CONSOLA SEGÚN FORMATO REQUERIDO
        std::cout << "======================================================\n";
        std::cout << "Funcion:\t" << t.name << "\n";
        std::cout << "Parametros:\t[c_sym=" << t.control[0] << ", UTH=" << upper << ", LTH=" << lower 
                  << ", c_up=" << t.control[3] << ", c_low=" << t.control[4] << "]\n";
        std::cout << "Senales:\treload=OK, use_sym=" << t.control[5] << ", use_lin=" << t.control[6] << "\n";
        std::cout << "Datos:\t\tSamples=" << sample_count << " | DLUT_elems=" << d_cap << " | ELUT_elems=" << active_depth << "\n";
        std::cout << "Precision:\tMSE = " << std::scientific << std::setprecision(6) << mse << "\n";
        std::cout << "Validacion:\t" << ((local_errors == 0) ? "[SUCCESS]" : "[FAILED - ERRORES CRITICOS]") << "\n";
    }

    std::cout << "======================================================\n";
    std::cout << "FINAL:\t\tTotal Errores = " << total_errors << "\n";
    std::cout << "======================================================\n";

    res_file.close();
    return (total_errors > 0) ? EXIT_FAILURE : EXIT_SUCCESS;
}