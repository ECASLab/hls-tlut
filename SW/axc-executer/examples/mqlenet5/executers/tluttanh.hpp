/*
 * Copyright (C) 2023-2026
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>

#ifdef WITH_OMP
#include <omp.h>
#endif

#include <data.hpp>
#include <kernels/activations.hpp>
#include <kernels/arithmetic.hpp>
#include <kernels/nonlinear.hpp>
#include <layer.hpp>
#include <runtime.hpp>

#include "../../../../api/tlut_api.hpp"  // Instancia de la API

inline TlutAccelerator& get_tlut_accelerator() {
    static TlutHardwareConfig hw_cfg;
    hw_cfg.max_samples = 50000000;

    static TlutAccelerator accel("../../HW/package.hw.v2/kernels_v2.xclbin", hw_cfg);
    return accel;
}

template <typename T>
static void TanhTlut(const T *input_data, T *output_data, const int size) {
  std::cout << "\n[DEBUG TLUT] Entrando a TanhTlut. Tamaño del lote: " << size << std::endl;
  
  auto& accel = get_tlut_accelerator();
  accel.load("tanh");

  uint16_t* in_map = accel.get_in_map();
  
  // Imprimir las primeras 5 muestras para ver el formato real de los bits
  std::cout << "[DEBUG TLUT] Muestras de entrada (Humano vs Bits .V):" << std::endl;
  for (int i = 0; i < std::min(size, 5); ++i) {
      float valor_humano = static_cast<float>(input_data[i]);
      uint16_t bits_puros = input_data[i].V;
      std::cout << "  Idx [" << i << "] -> Real: " << valor_humano 
                << " | Bits (.V): 0x" << std::hex << bits_puros << std::dec 
                << " | Sizeof de T: " << sizeof(input_data[i]) << " bytes" << std::endl;
  }

  for (int i = 0; i < size; ++i) {
    in_map[i] = input_data[i].V; 
  }

  accel.execute_process(size);

  const uint16_t* out_map = accel.get_out_map();
  
  std::cout << "[DEBUG TLUT] Muestras de salida del HW (Bits .V):" << std::endl;
  for (int i = 0; i < std::min(size, 5); ++i) {
      std::cout << "  Idx [" << i << "] -> HW extrajo: 0x" << std::hex << out_map[i] << std::dec << std::endl;
  }

  for (int i = 0; i < size; ++i) {
    output_data[i].V = out_map[i]; 
  }
}

template <typename T>
static void ExpTlut(const T *input_data, T *output_data, const int size) {
  // Lógica original de centrado (Safe Softmax)
  T max_val = 0.f;
  for (int i = 0; i < size; ++i) {
    max_val = input_data[i] > max_val ? input_data[i] : max_val;
  }
  for (int i = 0; i < size; ++i) {
    output_data[i] = input_data[i] - max_val;
  }

  std::cout << "\n[DEBUG SOFTMAX] Entrando a ExpTlut. Tamaño del lote: " << size << std::endl;
  std::cout << "[DEBUG SOFTMAX] Valor Máximo encontrado (Host): " << static_cast<float>(max_val) << std::endl;
  
  auto& accel = get_tlut_accelerator();
  accel.load("exp");

  uint16_t* in_map = accel.get_in_map();
  
  std::cout << "[DEBUG SOFTMAX] Muestras de entrada (Datos restados vs Bits .V a la FPGA):" << std::endl;
  for (int i = 0; i < std::min(size, 5); ++i) {
      float valor_humano = static_cast<float>(output_data[i]);
      uint16_t bits_puros = output_data[i].V;
      std::cout << "  Idx [" << i << "] -> Restado: " << valor_humano 
                << " | Bits (.V): 0x" << std::hex << bits_puros << std::dec << std::endl;
  }

  for (int i = 0; i < size; ++i) {
    in_map[i] = output_data[i].V; 
  }

  accel.execute_process(size);

  const uint16_t* out_map = accel.get_out_map();
  
  std::cout << "[DEBUG SOFTMAX] Muestras de salida del HW (Resultados de e^x en Bits .V):" << std::endl;
  for (int i = 0; i < std::min(size, 5); ++i) {
      std::cout << "  Idx [" << i << "] -> HW extrajo: 0x" << std::hex << out_map[i] << std::dec << std::endl;
  }

  for (int i = 0; i < size; ++i) {
    output_data[i].V = out_map[i];
  }
}

namespace Kernels {
namespace Exact {
/**
 * @brief Exact vector add implementation. It uses a complete version of the
 * addition using the system addition operator.
 *
 * @tparam T datatype
 * @tparam AO arithmetic operator
 */
template <class T, class AO = axc::arithmetic::exact::Add<T>>
struct ElementWise_TLUT {
    AO ao{};

    /** Number of samples included in a batch */
    int target_samples = 0;

    /** Number of inputs/outputs of the vectors to add */
    int target_outputs = 0;

    /** Activation used at the end of the layer */
    Activations activation;

    /**
     * @brief Functor operator()
     *
     * @param add_data pointer to the biases
     * @param input_data pointer to the input data
     * @param output_data pointer to the output data
     * @return Runtime
     */
    Runtime operator()(const T* add_data, const T* input_data, T* output_data) {
        if (!input_data || !output_data || !add_data) {
            return Runtime{Runtime::INVALID_PARAMETER, "Cannot execute kernel. The operators are nullptr"};
        }
#ifdef WITH_OMP
#pragma omp parallel for
#endif
        for (int sample = 0; sample < target_samples; ++sample) {
            for (int elem = 0; elem < target_outputs; ++elem) {
                int pos = elem + sample * target_outputs;
                T val = ao.operator()(add_data[elem], input_data[pos]);
                output_data[pos] = val;
            }
        }

        int size = target_outputs * target_samples;
        TanhTlut<T>(output_data, output_data, size);

        return Runtime{Runtime::OK, "Add1D success"};
    }
};

/**
 * @brief Exact matrix add implementation. It uses a complete version of the
 * addition using the system addition operator.
 *
 * @tparam T datatype
 * @tparam AO arithmetic operator
 */
template <class T, class AO = axc::arithmetic::exact::Add<T>>
struct ElementWise2D_TLUT {
    AO ao{};

    /** Number of samples included in a batch */
    int target_samples = 0;

    /** Number of channels included in an output sample */
    int target_channels = 0;

    /** Width of a sample. It must match the input */
    int target_width = 0;

    /** Height of a sample. It must match the input */
    int target_height = 0;

    /** Number of channels included in an input sample */
    int input_channels = 0;

    /** Width of a sample. It must match the output */
    int input_width = 0;

    /** Height of a sample. It must match the output */
    int input_height = 0;

    /** Activation used at the end of the layer */
    Activations activation;

    /**
     * @brief Functor operator()
     *
     * @param add_data pointer to the biases
     * @param input_data pointer to the input data
     * @param output_data pointer to the output data
     * @return Runtime
     */
    Runtime operator()(const T* add_data, const T* input_data, T* output_data) {
        if (!input_data || !output_data || !add_data) {
            return Runtime{Runtime::INVALID_PARAMETER, "Cannot execute kernel. The operators are nullptr"};
        }
#ifdef WITH_OMP
#pragma omp parallel for collapse(2)
#endif
        for (int sample = 0; sample < target_samples; ++sample) {
            for (int map = 0; map < target_channels; ++map) {
                T add_val = add_data[map];
                for (int y = 0; y < target_height; ++y) {
                    for (int x = 0; x < target_width; ++x) {
                        int input_x = x;
                        int input_y = y;
                        int input_c = map;

                        /* Get image pixel */
                        int64_t pos = input_x;                       /* x */
                        pos += input_y * input_width;                /* y */
                        pos += input_c * input_height * input_width; /* plane */
                        pos += input_height * input_width * input_channels *
                               sample; /* sample */
                        T val = ao(*(input_data + pos), add_val);

                        /* Save the result */
                        *(output_data + pos) = val;
                    }
                }
            }
        }

        int size = target_height * target_width * target_channels * target_samples;
        TanhTlut<T>(output_data, output_data, size);

        return Runtime{Runtime::OK, "Add2D success"};
    }
};

/**
 * @brief Exact softmax implementation. It uses the C++ STL std::exp under the
 * hood. This is improper for approximations
 *
 * @tparam T datatype
 * @tparam AO arithmetic operator
 */
template <class T>
struct SoftmaxTLUT {
    /** Target samples: number of samples of a batch. It must be equal to the
     * input samples */
    int target_samples = 0;
    /** Input values: number of input values */
    int input_values = 0;
    /** Output values: number of output values - it must match the input values */
    int target_outputs = 0;

    /**
     * @brief Functor operator()
     *
     * @param input_data pointer to the input data
     * @param output_data pointer to the output data
     * @return Runtime
     */
    Runtime operator()(const T* input_data, T* output_data) {
        if (!input_data || !output_data) {
            return Runtime{Runtime::INVALID_PARAMETER, "Cannot execute kernel. The operators are nullptr"};
        }

        int size = target_samples * input_values;
        T* exp_vector = new T[size];

        ExpTlut<T>(input_data, exp_vector, size);

        T den = 0.f;
        for (int sample = 0; sample < target_samples; ++sample) {
            for (int in = 0; in < input_values; ++in) {
                den = exp_vector[in + sample * input_values] + den;
            }

            /* Compute the probability */
            for (int out = 0; out < target_outputs; ++out) {
                output_data[out + sample * target_outputs] =
                    exp_vector[out + sample * input_values] / den;
            }
        }

        delete[] exp_vector;
        return Runtime{Runtime::OK, "Reduction"};  // Son muchos
    }
};
}  // namespace Exact
}  // namespace Kernels