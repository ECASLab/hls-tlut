/*
 * ----------------------------------------------------------------------------
 * Copyright (c) 2026 Sergio Porras Escobar <sporras29@estudiantec.cr>
 * ----------------------------------------------------------------------------
 */

#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>

// Forward declarations para ocultar dependencias de XRT al usuario final
namespace xrt { class device; class kernel; class bo; class run; }

/**
 * @brief Parámetros de Hardware para el bitstream del Acelerador t-LUT.
 */
struct TlutHardwareConfig {
    std::string kernel_name = "nla_top"; // Nombre configurable del kernel
    size_t max_samples = 100000;         // Máximo de elementos por transacción
    size_t dlut_words = 256;             // Tamaño BRAM D-LUT (palabras 128-bit)
    size_t elut_words = 1024;            // Tamaño BRAM E-LUT (palabras 128-bit)
    double fpga_freq_mhz = 250.0;        // Frecuencia de operación objetivo
    bool enable_profiling = false;       // Bandera para medir tiempos de hardware
};

class TlutAccelerator {
public:
    TlutAccelerator(const std::string& xclbin_path, 
                    int q_int, 
                    int q_frac, 
                    const TlutHardwareConfig& hw_config = TlutHardwareConfig(),
                    int device_id = 0);
    
    ~TlutAccelerator();

    void load(const std::string& func_name);
    std::vector<float> process(const std::vector<float>& input_data);
    
    double get_fpga_frequency_mhz() const;
    double get_last_load_duration_ns() const;
    double get_last_compute_duration_ns() const;

private:
    int q_int_;
    int q_frac_;
    double scale_;
    std::string format_folder_;
    
    TlutHardwareConfig hw_cfg_;

    // HW Control Registers
    int16_t lower_th_, upper_th_, c_lower_, c_upper_, c_sym_;
    uint8_t use_sym_, use_lin_;
    uint32_t active_depth_;

    // Registros de Telemetría
    double last_load_ns_ = 0.0;
    double last_compute_ns_ = 0.0;

    // Punteros opacos de XRT (Ocultan la librería subyacente)
    void* device_;
    void* kernel_;
    void* bo_in_;
    void* bo_out_;
    void* bo_d_;
    void* bo_e_;
    void* run_load_;    // Objeto run pre-instanciado para carga de LUT
    void* run_process_; // Objeto run pre-instanciado para cómputo

    void read_txt_to_vector(const std::string& filepath, std::vector<int>& vec);
};