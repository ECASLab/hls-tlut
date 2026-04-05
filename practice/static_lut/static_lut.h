#ifndef STATIC_LUT_H
#define STATIC_LUT_H

#include <ap_fixed.h>

// Formato Q6.10: 16 bits totales, 6 enteros, 10 fraccionales
typedef ap_fixed<16, 6> data_t;

// Identificadores de funciones
#define SEL_TANH     0
#define SEL_EXP      1
#define SEL_SQRT     2
#define SEL_EXP_NEG  3
#define SEL_SIGMOID  4
#define SEL_SOFTPLUS 5
#define SEL_GELU     6
#define SEL_ELU      7
#define SEL_RELU     8
#define SEL_SILU     9

// ========================================================
// VARIABLE GLOBAL: Selector de Síntesis
// Cambia este valor para sintetizar un hardware distinto
// ========================================================
#define ACTIVE_FUNCTION SEL_TANH

// ========================================================
// PARÁMETROS FÍSICOS SEGÚN LA FUNCIÓN SELECCIONADA
// ========================================================
#if ACTIVE_FUNCTION == SEL_TANH
    #define ACTIVE_LUT LUT_TANH
    #define LUT_DEPTH 4097
    #define RANGE_MIN (data_t)0.0
    #define RANGE_MAX (data_t)4.0
    #define IS_SYMMETRIC 1

#elif ACTIVE_FUNCTION == SEL_EXP
    #define ACTIVE_LUT LUT_EXP
    #define LUT_DEPTH 16385
    #define RANGE_MIN (data_t)-8.0
    #define RANGE_MAX (data_t)8.0
    #define IS_SYMMETRIC 0

#elif ACTIVE_FUNCTION == SEL_SQRT
    #define ACTIVE_LUT LUT_SQRT
    #define LUT_DEPTH 8193
    #define RANGE_MIN (data_t)0.0
    #define RANGE_MAX (data_t)8.0
    #define IS_SYMMETRIC 0

#elif ACTIVE_FUNCTION == SEL_EXP_NEG
    #define ACTIVE_LUT LUT_EXP_NEG
    #define LUT_DEPTH 8193
    #define RANGE_MIN (data_t)0.0
    #define RANGE_MAX (data_t)8.0
    #define IS_SYMMETRIC 0

#elif ACTIVE_FUNCTION == SEL_SIGMOID
    #define ACTIVE_LUT LUT_SIGMOID
    #define LUT_DEPTH 16385
    #define RANGE_MIN (data_t)-8.0
    #define RANGE_MAX (data_t)8.0
    #define IS_SYMMETRIC 0

#elif ACTIVE_FUNCTION == SEL_SOFTPLUS
    #define ACTIVE_LUT LUT_SOFTPLUS
    #define LUT_DEPTH 16385
    #define RANGE_MIN (data_t)-8.0
    #define RANGE_MAX (data_t)8.0
    #define IS_SYMMETRIC 0

#elif ACTIVE_FUNCTION == SEL_GELU
    #define ACTIVE_LUT LUT_GELU
    #define LUT_DEPTH 16385
    #define RANGE_MIN (data_t)-8.0
    #define RANGE_MAX (data_t)8.0
    #define IS_SYMMETRIC 0

#elif ACTIVE_FUNCTION == SEL_ELU
    #define ACTIVE_LUT LUT_ELU
    #define LUT_DEPTH 16385
    #define RANGE_MIN (data_t)-8.0
    #define RANGE_MAX (data_t)8.0
    #define IS_SYMMETRIC 0

#elif ACTIVE_FUNCTION == SEL_RELU
    // ReLU no requiere BRAM, es puramente lógica.
    #define IS_LOGIC_ONLY 1

#elif ACTIVE_FUNCTION == SEL_SILU
    #define ACTIVE_LUT LUT_SILU
    #define LUT_DEPTH 16385
    #define RANGE_MIN (data_t)-8.0
    #define RANGE_MAX (data_t)8.0
    #define IS_SYMMETRIC 0

#endif

#ifndef IS_LOGIC_ONLY
    #define IS_LOGIC_ONLY 0
#endif

extern "C" {
    void acelerador_nolineal(data_t x_in, data_t *y_out);
}

#endif