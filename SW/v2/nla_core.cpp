/*
 * ----------------------------------------------------------------------------
 * Copyright (c) 2026 Sergio Porras Escobar <sporras29@estudiantec.cr>
 * ----------------------------------------------------------------------------
 */

#include "nla_core.h"

// --- FUNCIONES WRAPPERS PARA ALIMENTAR EL DATAFLOW DESDE LA MEMORIA GLOBAL ---
void read_input(
    data_t* in_mem,
    hls::stream<data_t>& out_stream,
    ap_uint<32> num_samples
) {
#pragma HLS INLINE off
    for (ap_uint<32> i = 0; i < num_samples; ++i) {
#pragma HLS PIPELINE II = 1
#pragma HLS LOOP_TRIPCOUNT min=41 max=181 avg=120
        out_stream.write(in_mem[i]);
    }
}

void write_output(
    hls::stream<data_t>& in_stream,
    data_t* out_mem,
    ap_uint<32> num_samples
) {
#pragma HLS INLINE off
    for (ap_uint<32> i = 0; i < num_samples; ++i) {
#pragma HLS PIPELINE II = 1
#pragma HLS LOOP_TRIPCOUNT min=41 max=181 avg=120
        out_mem[i] = in_stream.read();
    }
}

// --- PIPELINES DE PROCESAMIENTO ---
void address_generation(
    hls::stream<data_t>& in_stream,
    hls::stream<s1_to_s2_t>& out_stream,
    nla_config_t config
) {
#pragma HLS INLINE off
    const ap_uint<32> num_samples = config.num_samples;
    if (num_samples == 0)
        return;

    for (ap_uint<32> i = 0; i < num_samples; ++i) {
#pragma HLS PIPELINE II = 1
#pragma HLS LOOP_TRIPCOUNT min=41 max=181 avg=120
        data_t x = in_stream.read();
        s1_to_s2_t out_pkt;
        out_pkt.en_mem = 0;
        out_pkt.address = 0;
        out_pkt.x_input = x;

        if (x < config.lower_threshold) {
            out_pkt.cmp_flag = 0;
        } else if (x > config.upper_threshold) {
            out_pkt.cmp_flag = config.use_lin ? ap_uint<2>(3) : ap_uint<2>(2);
        } else {
            out_pkt.cmp_flag = 1;
            out_pkt.en_mem = 1;
            bool is_negative = x[Q_TOT_WIDTH - 1];
            data_t x_abs = is_negative ? (data_t)(-x) : x;
            data_t x_lth = x - config.lower_threshold;
            data_t target = config.use_sym ? x_abs : x_lth;
            out_pkt.address = target.range(ADDR_WIDTH - 1, 0);
        }
        out_stream.write(out_pkt);
    }
}

void memory_access(
    hls::stream<s1_to_s2_t>& in_stream,
    hls::stream<s2_to_s3_t>& out_stream,
    dlut_t d_lut_bram[DLUT_DEPTH],
    elut_t e_lut_bram[ELUT_DEPTH],
    nla_config_t config
) {
#pragma HLS INLINE off
    const ap_uint<32> num_samples = config.num_samples;
    if (num_samples == 0)
        return;

    for (ap_uint<32> i = 0; i < num_samples; ++i) {
#pragma HLS PIPELINE II = 1
#pragma HLS LOOP_TRIPCOUNT min=41 max=181 avg=120
        s1_to_s2_t in_val = in_stream.read();
        s2_to_s3_t out_pkt;
        out_pkt.d_val = 0;
        out_pkt.e_val = 0;
        out_pkt.cmp_flag = in_val.cmp_flag;
        out_pkt.x_input = in_val.x_input;

        if (in_val.en_mem) {
            out_pkt.d_val = d_lut_bram[in_val.address.range(ADDR_WIDTH - 1, ELUT_WIDTH)];
            out_pkt.e_val = e_lut_bram[in_val.address];
        }
        out_stream.write(out_pkt);
    }
}

void output_reconstruction(
    hls::stream<s2_to_s3_t>& in_stream,
    hls::stream<data_t>& out_stream,
    nla_config_t config
) {
#pragma HLS INLINE off
    const ap_uint<32> num_samples = config.num_samples;
    if (num_samples == 0)
        return;

    for (ap_uint<32> i = 0; i < num_samples; ++i) {
#pragma HLS PIPELINE II = 1
#pragma HLS LOOP_TRIPCOUNT min=41 max=181 avg=120
        s2_to_s3_t in_val = in_stream.read();
        data_t out_data_val;
        data_t e_val_fix = 0;
        e_val_fix.range(ELUT_WIDTH - 1, 0) = in_val.e_val;
        data_t lut_out = in_val.d_val + e_val_fix;
        data_t c_sum_val = config.c_sym - lut_out;
        bool sign = in_val.x_input[Q_TOT_WIDTH - 1];
        data_t sym_out = (config.use_sym && sign) ? c_sum_val : lut_out;

        switch (in_val.cmp_flag) {
            case 0: out_data_val = config.c_lower; break;
            case 1: out_data_val = sym_out; break;
            case 2: out_data_val = config.c_upper; break;
            case 3: out_data_val = in_val.x_input; break;
            default: out_data_val = in_val.x_input; break;
        }
        out_stream.write(out_data_val);
    }
}

void reload_memory(
    axi_word_t* d_lut_mem,
    axi_word_t* e_lut_mem,
    dlut_t d_lut_bram[DLUT_DEPTH],
    elut_t e_lut_bram[ELUT_DEPTH],
    ap_uint<ADDR_WIDTH + 1> active_depth
) {
#pragma HLS INLINE off

    ap_uint<ADDR_WIDTH + 1> depth = (active_depth > MAX_DEPTH) ? (ap_uint<ADDR_WIDTH + 1>)MAX_DEPTH : active_depth;

    // Recarga E-LUT (32 elementos de 4-bit por ciclo)
    int e_chunks = (depth + E_RESHAPE_FACTOR - 1) / E_RESHAPE_FACTOR;
    for (int i = 0; i < e_chunks; ++i) {
#pragma HLS PIPELINE II = 1
#pragma HLS LOOP_TRIPCOUNT min=1 max=512 avg=512
        axi_word_t chunk = e_lut_mem[i];
        for (int j = 0; j < E_RESHAPE_FACTOR; ++j) {
#pragma HLS UNROLL
            int idx = i * E_RESHAPE_FACTOR + j;
            if (idx < MAX_DEPTH)
                e_lut_bram[idx] = chunk.range(j * ELUT_WIDTH + (ELUT_WIDTH - 1), j * ELUT_WIDTH);
        }
    }

    // Recarga D-LUT (8 elementos de 16-bit por ciclo)
    int d_total_elements = (depth + B_SIZE - 1) / B_SIZE;
    int d_chunks = (d_total_elements + D_RESHAPE_FACTOR - 1) / D_RESHAPE_FACTOR;
    for (int i = 0; i < d_chunks; ++i) {
#pragma HLS PIPELINE II = 1
#pragma HLS LOOP_TRIPCOUNT min=1 max=128 avg=128
        axi_word_t chunk = d_lut_mem[i];
        for (int j = 0; j < D_RESHAPE_FACTOR; ++j) {
#pragma HLS UNROLL
            int idx = i * D_RESHAPE_FACTOR + j;
            if (idx < DLUT_DEPTH) {
                ap_uint<DLUT_WIDTH> raw_val = chunk.range(j * DLUT_WIDTH + (DLUT_WIDTH - 1), j * DLUT_WIDTH);
                d_lut_bram[idx].range(DLUT_WIDTH - 1, 0) = raw_val;
            }
        }
    }
}

static void process_dataflow(
    data_t* in_data,
    data_t* out_data,
    dlut_t db[DLUT_DEPTH],
    elut_t eb[ELUT_DEPTH],
    nla_config_t cfg
) {
#pragma HLS INLINE off
#pragma HLS DATAFLOW

    hls::stream<data_t> s_in;
    hls::stream<s1_to_s2_t> s1_s2;
    hls::stream<s2_to_s3_t> s2_s3;
    hls::stream<data_t> s_out;

#pragma HLS STREAM variable = s_in depth = 32
#pragma HLS STREAM variable = s1_s2 depth = 32
#pragma HLS STREAM variable = s2_s3 depth = 32
#pragma HLS STREAM variable = s_out depth = 32

    read_input(in_data, s_in, cfg.num_samples);
    address_generation(s_in, s1_s2, cfg);
    memory_access(s1_s2, s2_s3, db, eb, cfg);
    output_reconstruction(s2_s3, s_out, cfg);
    write_output(s_out, out_data, cfg.num_samples);
}

extern "C" {
void nla_top(
    data_t* in_data,
    data_t* out_data,
    axi_word_t* d_lut_mem,
    axi_word_t* e_lut_mem,
    nla_config_t config
) {
#pragma HLS INTERFACE m_axi port = in_data offset = slave bundle = gmem_in depth = 100000
#pragma HLS INTERFACE m_axi port = out_data offset = slave bundle = gmem_out depth = 100000
#pragma HLS INTERFACE m_axi port = d_lut_mem offset = slave bundle = gmem_d depth = (DLUT_DEPTH / D_RESHAPE_FACTOR)
#pragma HLS INTERFACE m_axi port = e_lut_mem offset = slave bundle = gmem_e depth = (ELUT_DEPTH / E_RESHAPE_FACTOR)

#pragma HLS INTERFACE s_axilite port = in_data bundle = control
#pragma HLS INTERFACE s_axilite port = out_data bundle = control
#pragma HLS INTERFACE s_axilite port = d_lut_mem bundle = control
#pragma HLS INTERFACE s_axilite port = e_lut_mem bundle = control
#pragma HLS INTERFACE s_axilite port = config bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

    static dlut_t d_lut_bram[DLUT_DEPTH];
    static elut_t e_lut_bram[ELUT_DEPTH];

#pragma HLS ARRAY_RESHAPE variable = d_lut_bram type = cyclic factor = D_RESHAPE_FACTOR dim = 1
#pragma HLS ARRAY_RESHAPE variable = e_lut_bram type = cyclic factor = E_RESHAPE_FACTOR dim = 1
#pragma HLS BIND_STORAGE variable = d_lut_bram type = ram_s2p impl = bram
#pragma HLS BIND_STORAGE variable = e_lut_bram type = ram_s2p impl = bram

    if (config.reload_tlut)
        reload_memory(d_lut_mem, e_lut_mem, d_lut_bram, e_lut_bram, config.active_depth);
    else
        process_dataflow(in_data, out_data, d_lut_bram, e_lut_bram, config);
}
}