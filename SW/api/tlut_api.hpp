// tlut_api.hpp
/*
 * ----------------------------------------------------------------------------
 * API C++ para cargar TLUTs y comunicar un acelerador HW vía XRT.
 *
 * - El host trabaja con samples Q6.10 cuantizados como std::int16_t.
 * - La API no hace saturación, overflow checking ni conversión numérica.
 * - El dato se manda tal cual al FPGA; el hardware resuelve el comportamiento
 *   fuera de umbral según su diseño.
 * - Las LUTs se empaquetan internamente como palabras crudas de 128 bits.
 * ----------------------------------------------------------------------------
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace xrt {
class device;
class kernel;
class bo;
class run;
}

/**
 * @brief Configuración de alto nivel para el acelerador TLUT.
 */
struct TlutHardwareConfig {
    std::string kernel_name = "nla_top";
    std::size_t max_samples = 100000;
    std::size_t dlut_words = 256;
    std::size_t elut_words = 1024;
    double fpga_freq_mhz = 250.0;
    bool enable_profiling = false;

    int q_int = 6;
    int q_frac = 10;

    std::string tluts_dir = "/home/lleonvega/ecaslab/sergio.porras/TLUT_NLA/SW/tluts/";
};

/**
 * @brief API de usuario para cargar LUTs y ejecutar el acelerador.
 *
 * Uso típico:
 *  1. Construir la clase con el xclbin.
 *  2. Llamar load("nombre_funcion").
 *  3. Escribir samples Q6.10 en get_in_map().
 *  4. Llamar execute_process(N).
 *  5. Leer resultados desde get_out_map().
 */
class TlutAccelerator {
public:
    using sample_t = std::int16_t;

    explicit TlutAccelerator(const std::string& xclbin_path,
                             const TlutHardwareConfig& hw_config = TlutHardwareConfig(),
                             int device_id = 0);

    ~TlutAccelerator();

    TlutAccelerator(const TlutAccelerator&) = delete;
    TlutAccelerator& operator=(const TlutAccelerator&) = delete;
    TlutAccelerator(TlutAccelerator&&) = delete;
    TlutAccelerator& operator=(TlutAccelerator&&) = delete;

    /**
     * @brief Carga la configuración de LUTs de una función concreta.
     *
     * Estructura esperada:
     *   tluts_dir / Q<int>_<frac> / func_name /
     *
     * Archivos:
     *   - dlut.txt
     *   - elut.txt
     *   - control.txt
     *
     * control.txt debe contener 9 enteros, en este orden:
     *   lower_th, upper_th, c_lower, c_upper, c_sym, use_sym, use_lin,
     *   dlut_lines, elut_lines
     */
    void load(const std::string& func_name);

    /**
     * @brief Obtiene el buffer de entrada mapeado en memoria.
     *
     * El usuario escribe directamente samples Q6.10 cuantizados como int16_t.
     */
    sample_t* get_in_map();

    /**
     * @brief Obtiene el buffer de salida mapeado en memoria.
     *
     * Debe leerse después de execute_process().
     */
    const sample_t* get_out_map() const;

    /**
     * @brief Ejecuta el kernel con N samples.
     *
     * No se hacen conversiones ni protección de overflow/underflow.
     */
    void execute_process(std::size_t samples_count);

    double get_fpga_frequency_mhz() const noexcept;
    double get_last_load_duration_ns() const noexcept;
    double get_last_compute_duration_ns() const noexcept;
    std::size_t max_samples() const noexcept { return hw_cfg_.max_samples; }

private:
    TlutHardwareConfig hw_cfg_;
    std::string format_folder_;

    std::int16_t lower_th_{0};
    std::int16_t upper_th_{0};
    std::int16_t c_lower_{0};
    std::int16_t c_upper_{0};
    std::int16_t c_sym_{0};
    std::uint8_t use_sym_{0};
    std::uint8_t use_lin_{0};
    std::uint32_t active_depth_{0};

    double last_load_ns_{0.0};
    double last_compute_ns_{0.0};

    xrt::device* device_{nullptr};
    xrt::kernel* kernel_{nullptr};
    xrt::bo* bo_in_{nullptr};
    xrt::bo* bo_out_{nullptr};
    xrt::bo* bo_d_{nullptr};
    xrt::bo* bo_e_{nullptr};
    xrt::run* run_load_{nullptr};
    xrt::run* run_process_{nullptr};

    void execute_run(xrt::run& run_obj, double& telemetry_ns);
};