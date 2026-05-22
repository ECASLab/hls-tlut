/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <fal-accels/examples/convolution/stream-convolution-tb.hpp>
#include <fal-accels/examples/convolution/stream-convolution.hpp>

/**
 * Uploads the image from a batch array to the HLS Stream
 * @param input_batch batch to copy to the stream
 * @param stream_input stream port to emit the data
 */
static void UploadImage(const DataType input_batch[kRowsWindow][kWindowSize],
                        StreamPort &stream_input) {  // NOLINT
  PayloadType payload{0};
  int payl_elems = 0;

  for (int y = 0; y < kRowsWindow; ++y) {
    for (int x = 0; x < kWindowSize; ++x) {
      /* If the elements are complete */
      if (kTotalPacketsPerPayload == payl_elems) {
        StreamPayload spayload{};
        spayload.data = payload;
        spayload.keep = -1;
        spayload.last = 0;
        stream_input.write(spayload);
        payl_elems = 0;
        payload = 0;
      }

      int upper = Q_BW * (payl_elems + 1) - 1;
      int lower = Q_BW * payl_elems;
      payload.range(upper, lower) = input_batch[y][x].V;
      ++payl_elems;
    }
  }

  /* Last write */
  if (payl_elems != 0) {
    StreamPayload spayload{};
    spayload.data = payload;
    spayload.keep = -1;
    spayload.last = 1;
    stream_input.write(spayload);
    payl_elems = 0;
  }
}

/**
 * Downloads the image from the HLS Stream to a batch array
 * @param stream_output stream port to receive the data from
 * @param output_batch batch to copy from the stream
 */
static void DownloadImage(StreamPort &stream_output,  // NOLINT
                          DataType output_batch[kRowsOutput][kOutputSize]) {
  const int kTotalPackets = kRowsOutput * kOutputSize / kTotalPacketsPerPayload;
  int x = 0;
  int y = 0;
  StreamPayload spayload;

  do {
    spayload = stream_output.read();

    /* Read the packets */
    for (int p = 0; p < kTotalPacketsPerPayload; ++p) {
      /* Read and set */
      int upper = Q_BW * (p + 1) - 1;
      int lower = Q_BW * p;
      output_batch[y][x].V = spayload.data.range(upper, lower);
      /* Check boundaries */
      if (++x >= kOutputSize) {
        ++y;
        x = 0;
      }
    }
  } while (!spayload.user.range(0, 0));
}

/**
 * @brief Copies from a big matrix to a window matrix
 *
 * @tparam TO type of the data
 * @param input input big matrix
 * @param output output window matrix
 * @param x0 offset in X within the big matrix
 * @param y0 offset in Y within the big matrix
 * @param roiw width of the window matrix
 * @param roih height of the window matrix
 * @param ipitch pitch of the input matrix
 * @param iheight height of the input matrix
 * @param opitch pitch of the output matrix
 */
template <typename TO>
static void CopyMatToArray(const TO *input, TO *output, const uint x0,
                           const uint y0, const uint roiw, const uint roih,
                           const uint ipitch, const uint iheight,
                           const uint opitch) {
  for (uint i{0}; i < roih; ++i) {
    for (uint j{0}; j < roiw; ++j) {
      const uint xi = (j + x0);
      const uint yi = (i + y0);
      const bool tlbounds = xi >= 0u && yi >= 0u;
      const bool brbounds = xi < ipitch && yi < iheight;
      output[i * opitch + j] =
          tlbounds && brbounds ? input[xi + yi * ipitch] : TO{0};
    }
  }
}

/**
 * @brief Copies from a window matrix to a big matrix
 *
 * @tparam TO data type of the matrices
 * @tparam acc output matrix accumulation
 * @param input input window matrix
 * @param output output big matrix
 * @param x0 offset in X w.r.t. the output matrix
 * @param y0 offset in Y w.r.t. the output matrix
 * @param roiw width of the input matrix
 * @param roih height of the input matrix
 * @param ipitch pitch of the input matrix
 * @param opitch pitch of the output matrix
 * @param oheight height of the output matrix
 */
template <typename TO, bool acc = true>
static void CopyMatFromArray(const TO *input, TO *output, const uint x0,
                             const uint y0, const uint roiw, const uint roih,
                             const uint ipitch, const uint opitch,
                             const uint oheight) {
  for (uint i{0}; i < roih; ++i) {
    for (uint j{0}; j < roiw; ++j) {
      const uint xo = (j + x0);
      const uint yo = (i + y0);
      const bool tlbounds = xo >= 0u && yo >= 0u;
      const bool brbounds = xo < opitch && yo < oheight;
      if (!tlbounds || !brbounds) return;
      if (acc) {
        output[xo + yo * opitch] += input[i * ipitch + j];
      } else {
        output[xo + yo * opitch] = input[i * ipitch + j];
      }
    }
  }
}

/**
 * @brief Loads the kernel, centering it according to the accelerator
 * kernel size
 *
 * @tparam TO data type of the kernel
 * @param input input kernel: to be centred. It must be smaller or equal in
 * dimensions to the output kernel
 * @param output accelerator's kernel
 * @param kmaxwidth accelerator's kernel width
 * @param kmaxheight accelerator's kernel height
 * @param kwidth kernel width
 * @param kheight kernel height
 */
template <typename TO>
static void LoadKernel(const TO *input, TO *output, const uint kmaxwidth,
                       const uint kmaxheight, const uint kwidth,
                       const uint kheight) {
  const uint offset_x = (kmaxwidth - kwidth) >> 1;
  const uint offset_y = (kmaxheight - kheight) >> 1;

  for (int i{0}; i < kheight; ++i) {
    for (int j{0}; j < kwidth; ++j) {
      const uint xo = j + offset_x;
      const uint yo = (i + offset_y) * kmaxwidth;
      const uint xi = j;
      const uint yi = i * kwidth;
      output[xo + yo] = input[xi + yi];
    }
  }
}
