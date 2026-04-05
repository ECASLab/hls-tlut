#include "static_lut.h"

// Si no es solo lógica, incluimos las tablas autogeneradas
#if !IS_LOGIC_ONLY
#include "t-luts.h" 
#endif

extern "C" {
void acelerador_nolineal(data_t x_in, data_t *y_out) {
#pragma HLS INTERFACE s_axilite port=x_in
#pragma HLS INTERFACE s_axilite port=y_out
#pragma HLS INTERFACE s_axilite port=return

    data_t res = 0;

// ========================================================
// CASO 1: Lógica Pura (Ej. ReLU)
// No se infieren memorias, solo comparadores.
// ========================================================
#if IS_LOGIC_ONLY
    if (x_in > (data_t)0.0) {
        res = x_in;
    } else {
        res = (data_t)0.0;
    }
    *y_out = res;

// ========================================================
// CASO 2: Funciones Simétricas (Ej. Tanh)
// ========================================================
#elif IS_SYMMETRIC
    bool is_neg = x_in[15];
    data_t abs_x = is_neg ? (data_t)(-x_in) : x_in;

    if (abs_x >= RANGE_MAX) {
        res.range() = ACTIVE_LUT[LUT_DEPTH - 1];
    } else {
        // En Q6.10, la extracción de 13 bits (12 a 0) permite contar hasta 8191.
        // Físicamente, conecta los 13 cables LSB al puerto de dirección de la BRAM.
        ap_uint<13> index = abs_x(12, 0);
        res.range() = ACTIVE_LUT[index];
    }
    
    *y_out = is_neg ? (data_t)(-res) : res;

// ========================================================
// CASO 3: Funciones Asimétricas (Ej. Sigmoid, Exp, Sqrt)
// ========================================================
#else
    if (x_in >= RANGE_MAX) {
        res.range() = ACTIVE_LUT[LUT_DEPTH - 1];
    
    } else if (x_in <= RANGE_MIN) {
        // Para funciones como Sqrt (cuyo RANGE_MIN es 0), esto protege
        // de evaluar números negativos y los satura a la posición 0 de la LUT.
        res.range() = ACTIVE_LUT[0];
    
    } else {
        // Al restar el límite inferior, desplazamos la ventana para que 
        // el índice más bajo comience siempre exactamente en la posición 0.
        data_t offset_x = x_in - RANGE_MIN;
        
        // Extracción de 15 bits para soportar profundidades de hasta 32767.
        ap_uint<15> index = offset_x(14, 0);
        res.range() = ACTIVE_LUT[index];
    }
    
    *y_out = res;
#endif

}
}