/*
 * ----------------------------------------------------------------------------
 * Copyright (c) 2026 Sergio Porras Escobar <sporras29@estudiantec.cr>
 * ----------------------------------------------------------------------------
 */

#ifndef NLA_CORE_H
#define NLA_CORE_H

#include <ap_fixed.h>
#include <ap_int.h>
#include <hls_stream.h>

const int Q_TOT_WIDTH = 16;
const int Q_INT_WIDTH = 6;

// [HARDWARE] Datos puros. Sin redondeo/saturación implícitos.
typedef ap_fixed<Q_TOT_WIDTH, Q_INT_WIDTH, AP_TRN, AP_WRAP> data_t;

// [NUEVO] AXI-Stream súper ligero sin TLAST
struct axis_t {
    data_t data;
};

const int ADDR_WIDTH = 14;
const int MAX_DEPTH = (1 << ADDR_WIDTH);

const int ELUT_DEPTH = MAX_DEPTH;
const int ELUT_WIDTH = 3;

const int B_SIZE = (1 << ELUT_WIDTH);
const int DLUT_DEPTH = (MAX_DEPTH / B_SIZE);
const int DLUT_WIDTH = Q_TOT_WIDTH;

typedef data_t dlut_t;
typedef ap_uint<ELUT_WIDTH> elut_t;
typedef ap_uint<ADDR_WIDTH> addr_t;

struct nla_config_t {
    data_t c_sym;
    data_t upper_threshold;
    data_t lower_threshold;
    data_t c_upper;
    data_t c_lower;

    ap_uint<1> reload_tlut;
    ap_uint<1> use_sym;
    ap_uint<1> use_lin;

    // Longitud del lote definida por software.
    ap_uint<32> num_samples;

    // Profundidad válida a recargar desde DDR hacia BRAM.
    ap_uint<ADDR_WIDTH + 1> active_depth;
};

// Structs internos ultra-optimizados (Sin señal 'last')
struct s1_to_s2_t {
    addr_t address;
    bool en_mem;
    ap_uint<2> cmp_flag;
    data_t x_input;
};

struct s2_to_s3_t {
    dlut_t d_val;
    elut_t e_val;
    ap_uint<2> cmp_flag;
    data_t x_input;
};

void address_generation(
    hls::stream<axis_t>& in_stream,
    hls::stream<s1_to_s2_t>& out_stream,
    nla_config_t config
);

void memory_access(
    hls::stream<s1_to_s2_t>& in_stream,
    hls::stream<s2_to_s3_t>& out_stream,
    dlut_t d_lut_bram[DLUT_DEPTH],
    elut_t e_lut_bram[ELUT_DEPTH],
    nla_config_t config
);

void output_reconstruction(
    hls::stream<s2_to_s3_t>& in_stream,
    hls::stream<axis_t>& out_stream,
    nla_config_t config
);

void reload_memory(
    dlut_t* d_lut_mem,
    elut_t* e_lut_mem,
    dlut_t d_lut_bram[DLUT_DEPTH],
    elut_t e_lut_bram[ELUT_DEPTH],
    ap_uint<ADDR_WIDTH + 1> active_depth
);

extern "C" {
    void nla_top(
        hls::stream<axis_t>& in_data,
        hls::stream<axis_t>& out_data,
        dlut_t* d_lut_mem,
        elut_t* e_lut_mem,
        nla_config_t config
    );
}

#endif  // NLA_CORE_H