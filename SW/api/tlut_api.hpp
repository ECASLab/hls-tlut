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

// Forward declarations to hide XRT dependencies from the user
namespace xrt { class device; class kernel; class bo; class run; }

/**
 * @brief Hardware parameters for the TLUT Accelerator bitstream.
 */
struct TlutHardwareConfig {
    std::string kernel_name = "nla_top"; // Configurable kernel name
    size_t max_samples = 100000;         // Maximum elements per transaction
    size_t dlut_words = 256;             // Size of the D-LUT BRAM (in 128-bit words)
    size_t elut_words = 1024;            // Size of the E-LUT BRAM (in 128-bit words)
    double fpga_freq_mhz = 250.0;        // Target operating frequency
    bool enable_profiling = false;       // Feature flag for internal hardware timing
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

    // Telemetry Registers
    double last_load_ns_ = 0.0;
    double last_compute_ns_ = 0.0;

    // Opaque XRT pointers
    void* device_;
    void* kernel_;
    void* bo_in_;
    void* bo_out_;
    void* bo_d_;
    void* bo_e_;
    void* run_load_;    // Pre-instantiated run object for LUT load
    void* run_process_; // Pre-instantiated run object for compute

    void read_txt_to_vector(const std::string& filepath, std::vector<int>& vec);
};