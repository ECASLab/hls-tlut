/*
 * ----------------------------------------------------------------------------
 * Copyright (c) 2026 Sergio Porras Escobar <sporras29@estudiantec.cr>
 * ----------------------------------------------------------------------------
 */

#pragma once

#include <string>
#include <cstdint>
#include <stdexcept>

// Forward declarations para mantener la cabecera limpia
namespace xrt { class device; class kernel; class bo; class run; }

struct TlutHardwareConfig {
    std::string kernel_name = "nla_top";
    size_t max_samples = 100000;
    size_t dlut_words = 256;
    size_t elut_words = 1024;
    double fpga_freq_mhz = 250.0;
    bool enable_profiling = false;
    
    int q_int = 6;
    int q_frac = 10;
    
    std::string tluts_dir = "/home/lleonvega/ecaslab/sergio.porras/TLUT_NLA/SW/tluts/"; 
};

class TlutAccelerator {
public:
    explicit TlutAccelerator(const std::string& xclbin_path, 
                             const TlutHardwareConfig& hw_config = TlutHardwareConfig(),
                             int device_id = 0);
    
    ~TlutAccelerator();

    void load(const std::string& func_name);
    
    // ========================================================================
    // Acceso a Memoria Zero-Copy
    // ========================================================================
    uint16_t* get_in_map();
    const uint16_t* get_out_map();
    void execute_process(size_t samples_count);

    // Telemetría
    double get_fpga_frequency_mhz() const;
    double get_last_load_duration_ns() const;
    double get_last_compute_duration_ns() const;

private:
    TlutHardwareConfig hw_cfg_;
    std::string format_folder_;

    int16_t lower_th_, upper_th_, c_lower_, c_upper_, c_sym_;
    uint8_t use_sym_, use_lin_;
    uint32_t active_depth_;

    double last_load_ns_ = 0.0;
    double last_compute_ns_ = 0.0;

    void* device_;
    void* kernel_;
    void* bo_in_;
    void* bo_out_;
    void* bo_d_;
    void* bo_e_;
    void* run_load_;    
    void* run_process_; 

    void execute_run(void* run_obj, double& telemetry_ns); 
};