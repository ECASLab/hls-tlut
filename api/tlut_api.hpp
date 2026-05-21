#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>

// Forward declarations to hide XRT dependencies from the user
namespace xrt { class device; class kernel; class bo; }

/**
 * @brief Hardware parameters for the TLUT Accelerator bitstream.
 */
struct TlutHardwareConfig {
    size_t max_samples = 100000;  // Maximum elements per transaction
    size_t dlut_words = 256;      // Size of the D-LUT BRAM (in 128-bit words)
    size_t elut_words = 1024;     // Size of the E-LUT BRAM (in 128-bit words)
    double fpga_freq_mhz = 250.0; // Target operating frequency
};

class TlutAccelerator {
public:
    /**
     * @brief Initializes the FPGA accelerator with specific hardware limits.
     * @param xclbin_path Path to the hardware bitstream.
     * @param q_int Number of integer bits in Q format.
     * @param q_frac Number of fractional bits in Q format.
     * @param hw_config Hardware sizing parameters.
     * @param device_id PCIe device ID (default: 0).
     */
    TlutAccelerator(const std::string& xclbin_path, 
                    int q_int, 
                    int q_frac, 
                    const TlutHardwareConfig& hw_config = TlutHardwareConfig(),
                    int device_id = 0);
    
    ~TlutAccelerator();

    /**
     * @brief Loads the selected activation function into the FPGA BRAMs.
     * @param func_name Name of the function (e.g., "tanh", "sigmoid").
     */
    void load(const std::string& func_name);

    /**
     * @brief Sends an array of floats to the FPGA, processes them, and returns the result.
     * @param input_data Vector of floating-point values to be processed.
     * @return Vector of processed floating-point values.
     */
    std::vector<float> process(const std::vector<float>& input_data);

    /**
     * @brief Retrieves the current hardware operating frequency.
     */
    double get_fpga_frequency_mhz() const;

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

    // Opaque XRT pointers
    void* device_;
    void* kernel_;
    void* bo_in_;
    void* bo_out_;
    void* bo_d_;
    void* bo_e_;

    void read_txt_to_vector(const std::string& filepath, std::vector<int>& vec);
};