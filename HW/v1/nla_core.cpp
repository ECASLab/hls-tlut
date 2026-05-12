/*
 * ----------------------------------------------------------------------------
 * Copyright (c) 2026 Sergio Porras Escobar <sporras29@estudiantec.cr>
 * ----------------------------------------------------------------------------
 */

#include "nla_core.h"

void address_generation(
    hls::stream<axis_t>& in_stream,
    hls::stream<s1_to_s2_t>& out_stream,
    nla_config_t config
) {
#pragma HLS INLINE off

    const ap_uint<32> num_samples = config.num_samples;
    if (num_samples == 0) return;

    for (ap_uint<32> i = 0; i < num_samples; ++i) {
#pragma HLS PIPELINE II = 1
#pragma HLS LOOP_TRIPCOUNT min=1 max=100000 avg=81

        axis_t in_val = in_stream.read();
        data_t x = in_val.data;

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
    if (num_samples == 0) return;

    for (ap_uint<32> i = 0; i < num_samples; ++i) {
#pragma HLS PIPELINE II = 1
#pragma HLS LOOP_TRIPCOUNT min=1 max=100000 avg=81

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
    hls::stream<axis_t>& out_stream,
    nla_config_t config
) {
#pragma HLS INLINE off

    const ap_uint<32> num_samples = config.num_samples;
    if (num_samples == 0) return;

    for (ap_uint<32> i = 0; i < num_samples; ++i) {
#pragma HLS PIPELINE II = 1
#pragma HLS LOOP_TRIPCOUNT min=1 max=100000 avg=81

        s2_to_s3_t in_val = in_stream.read();
        axis_t out_pkt;

        data_t e_val_fix = 0;
        e_val_fix.range(ELUT_WIDTH - 1, 0) = in_val.e_val;

        data_t lut_out = in_val.d_val + e_val_fix;
        data_t c_sum_val = config.c_sym - lut_out;

        bool sign = in_val.x_input[Q_TOT_WIDTH - 1];
        data_t sym_out = (config.use_sym && sign) ? c_sum_val : lut_out;

        switch (in_val.cmp_flag) {
            case 0: out_pkt.data = config.c_lower; break;
            case 1: out_pkt.data = sym_out; break;
            case 2: out_pkt.data = config.c_upper; break;
            case 3: out_pkt.data = in_val.x_input; break;
            default: out_pkt.data = in_val.x_input; break;
        }

        out_stream.write(out_pkt);
    }
}

void reload_memory(
    dlut_t* d_lut_mem,
    elut_t* e_lut_mem,
    dlut_t d_lut_bram[DLUT_DEPTH],
    elut_t e_lut_bram[ELUT_DEPTH],
    ap_uint<ADDR_WIDTH + 1> active_depth
) {
#pragma HLS INLINE off

    ap_uint<ADDR_WIDTH + 1> depth = (active_depth > MAX_DEPTH)
        ? (ap_uint<ADDR_WIDTH + 1>)MAX_DEPTH
        : active_depth;

    ap_uint<ADDR_WIDTH + 1> d_cap = (depth + B_SIZE - 1) >> ELUT_WIDTH;

    for (ap_uint<ADDR_WIDTH + 1> i = 0; i < depth; ++i) {
#pragma HLS PIPELINE II = 1
#pragma HLS LOOP_TRIPCOUNT min=1 max=16384 avg=12289
        e_lut_bram[i] = e_lut_mem[i];
    }

    for (ap_uint<ADDR_WIDTH + 1> i = 0; i < d_cap; ++i) {
#pragma HLS PIPELINE II = 1
#pragma HLS LOOP_TRIPCOUNT min=1 max=2048 avg=1537
        d_lut_bram[i] = d_lut_mem[i];
    }
}

static void process_dataflow(
    hls::stream<axis_t>& in,
    hls::stream<axis_t>& out,
    dlut_t db[DLUT_DEPTH],
    elut_t eb[ELUT_DEPTH],
    nla_config_t cfg
) {
#pragma HLS INLINE off
#pragma HLS DATAFLOW
#pragma HLS STABLE variable=cfg
#pragma HLS STABLE variable=db
#pragma HLS STABLE variable=eb

    hls::stream<s1_to_s2_t> s1_s2;
    hls::stream<s2_to_s3_t> s2_s3;

#pragma HLS STREAM variable=s1_s2 depth=32
#pragma HLS STREAM variable=s2_s3 depth=32

    address_generation(in, s1_s2, cfg);
    memory_access(s1_s2, s2_s3, db, eb, cfg);
    output_reconstruction(s2_s3, out, cfg);
}

extern "C" {
    void nla_top(
        hls::stream<axis_t>& in_data,
        hls::stream<axis_t>& out_data,
        dlut_t* d_lut_mem,
        elut_t* e_lut_mem,
        nla_config_t config
    ) {
#pragma HLS INTERFACE axis port=in_data
#pragma HLS INTERFACE axis port=out_data

#pragma HLS INTERFACE m_axi port=d_lut_mem offset=slave bundle=gmem_d depth=DLUT_DEPTH
#pragma HLS INTERFACE m_axi port=e_lut_mem offset=slave bundle=gmem_e depth=ELUT_DEPTH

#pragma HLS INTERFACE s_axilite port=d_lut_mem bundle=control
#pragma HLS INTERFACE s_axilite port=e_lut_mem bundle=control
#pragma HLS INTERFACE s_axilite port=config bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

#pragma HLS AGGREGATE variable=config compact=bit

        static dlut_t d_lut_bram[DLUT_DEPTH];
        static elut_t e_lut_bram[ELUT_DEPTH];

#pragma HLS BIND_STORAGE variable=d_lut_bram type=ram_s2p impl=bram
#pragma HLS BIND_STORAGE variable=e_lut_bram type=ram_s2p impl=bram

        if (config.reload_tlut) {
            reload_memory(d_lut_mem, e_lut_mem, d_lut_bram, e_lut_bram, config.active_depth);
        } else {
            process_dataflow(in_data, out_data, d_lut_bram, e_lut_bram, config);
        }
    }
}