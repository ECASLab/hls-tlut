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

// Forward declarations para mantener la cabecera limpia de dependencias de XRT
namespace xrt { class device; class kernel; class bo; class run; }

/**
 * @brief Parámetros de Hardware y Configuración para el Acelerador t-LUT.
 */
struct TlutHardwareConfig {
    std::string kernel_name = "nla_top"; // Nombre configurable del kernel
    size_t max_samples = 100000;         // Máximo de elementos por transacción
    size_t dlut_words = 256;             // Tamaño BRAM D-LUT (palabras 128-bit)
    size_t elut_words = 1024;            // Tamaño BRAM E-LUT (palabras 128-bit)
    double fpga_freq_mhz = 250.0;        // Frecuencia de operación objetivo
    bool enable_profiling = false;       // Bandera para medir tiempos de hardware
    
    // Formato numérico de punto fijo Q (16 bits totales)
    int q_int = 6;                       // Bits enteros (incluye bit de signo)
    int q_frac = 10;                     // Bits fraccionarios
    
    // Directorio base de las tablas (Permite portabilidad de ejecución)
    std::string tluts_dir = "/home/lleonvega/ecaslab/sergio.porras/TLUT_NLA/SW/tluts/"; 
};

class TlutAccelerator {
public:
    explicit TlutAccelerator(const std::string& xclbin_path, 
                             const TlutHardwareConfig& hw_config = TlutHardwareConfig(),
                             int device_id = 0);
    
    ~TlutAccelerator();

    // Carga la tabla de búsqueda (t-LUT) en la FPGA
    void load(const std::string& func_name);
    
    // Procesamiento de Inferencia Ultra-Rápido (Zero-cost HW Directo).
    // Utiliza punteros crudos para aceptar arreglos de C, memoria dinámica o vectores sin penalización de copiado.
    void process(const int16_t* input_ptr, int16_t* output_ptr, size_t samples_count);
    
    // Funciones de Telemetría
    double get_fpga_frequency_mhz() const;
    double get_last_load_duration_ns() const;
    double get_last_compute_duration_ns() const;

private:
    TlutHardwareConfig hw_cfg_;
    std::string format_folder_;

    // HW Control Registers
    int16_t lower_th_, upper_th_, c_lower_, c_upper_, c_sym_;
    uint8_t use_sym_, use_lin_;
    uint32_t active_depth_;

    // Registros de Telemetría
    double last_load_ns_ = 0.0;
    double last_compute_ns_ = 0.0;

    // Punteros opacos de XRT (Oculta la implementación cruda)
    void* device_;
    void* kernel_;
    void* bo_in_;
    void* bo_out_;
    void* bo_d_;
    void* bo_e_;
    void* run_load_;    
    void* run_process_; 

    // Métodos auxiliares
    void read_txt_to_vector(const std::string& filepath, std::vector<int>& vec);
    void execute_run(void* run_obj, double& telemetry_ns); 
};