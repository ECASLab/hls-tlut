/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#include <iostream>
#include <opencv2/opencv.hpp>

#include "accel_stream.hpp"
#include "accel_stream_tb.hpp"
#include "opencv_utils.hpp"
#include "utils/image_stats.hpp"
#include "utils/printmatrix.hpp"

static const int kElemsPerPayload = 64 / Q_BW;

/**
 * Uploads the image from a batch array to the HLS Stream
 * @param input_batch batch to copy to the stream
 * @param stream_input stream port to emit the data
 */
static void UploadImage(const DataType input_batch[kRowsWindow][kWindowSize],
                        StreamPort &stream_input) {
  PayloadType payload{0};
  int payl_elems = 0;

  for (int y = 0; y < kRowsWindow; ++y) {
    for (int x = 0; x < kWindowSize; ++x) {
      /* If the elements are complete */
      if (kElemsPerPayload == payl_elems) {
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
static void DownloadImage(StreamPort &stream_output,
                          DataType output_batch[kRowsOutput][kOutputSize]) {
  const int kTotalPackets = kRowsOutput * kOutputSize / kElemsPerPayload;
  int x = 0;
  int y = 0;
  int packets = 0;
  StreamPayload spayload;

  do {
    spayload = stream_output.read();

    /* Read the packets */
    for (int p = 0; p < kElemsPerPayload; ++p) {
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

  } while (!spayload.last && (packets++ < kTotalPackets));
}

int main(int argc, char **argv) {
  const std::string kDefaultInputImage{"misc/lenna"};
  const std::string kDefaultOutputImage{"misc/lenna-output"};
  const bool kEnablePrinting = false;

  int ret = 0;
  DataType output_batch[kRowsOutput][kOutputSize];
  DataType input_batch[kRowsWindow][kWindowSize];

  /* -- Get image -- */
  cv::Mat input_img, output_hw, output_sw, kernel_sw;

  /* Get the input image */
  if (argc < 2) {
    input_img = cv::imread(kDefaultInputImage, 0);
  } else {
    input_img = cv::imread(argv[1], 0);
  }

  /* Check it */
  if (input_img.empty()) {
    std::cerr << "Cannot open the image" << std::endl;
    return -1;
  }

  /* Streaming ports */
  StreamPort stream_input{}, stream_output{};

  /* Execution params */
  DECL_EXE_PARAMS_TB;

  /* Capabilities */
  DECL_CAPS_TB;

  /* Kernels */
  DataType kernels[kKernelSize * kKernelSize];

  /* -- Write properties -- */
  WRITE_EXE_PARAM_TB(input_width, kWindowSize);    // Changed
  WRITE_EXE_PARAM_TB(input_height, kRowsWindow);   // Changed
  WRITE_EXE_PARAM_TB(output_width, kOutputSize);   // Changed
  WRITE_EXE_PARAM_TB(output_height, kRowsOutput);  // Changed
  WRITE_EXE_PARAM_TB(kernel_size, kKernelSize);    // Changed

  /* -- Execute -- */
  WRITE_EXE_PARAM_TB(execution_mode, static_cast<int>(ExecutionModes::CONFIG));
  EXECUTE_ACCEL;

  /* -- Read properties back -- */
  READ_PROPS;

  /* -- Set kernel -- */
  kernel_sw = get_filter(Q_K);
  CopyMatToArray<double, false>(kernel_sw, kernels, cv::Rect{0, 0, Q_K, Q_K});
  std::cout << "Kernel: \n" << kernel_sw << std::endl;

  /* -- Run with kernel -- */
  WRITE_EXE_PARAM_TB(execution_mode,
                     static_cast<int>(ExecutionModes::LOAD_KERNEL));
  EXECUTE_ACCEL;

  /* -------------------------------------------------------- */
  /* --------------------- Hardware run --------------------- */
  /* -------------------------------------------------------- */

  /* -- Create padded img -- */
  const int offset = Q_K / 2;
  cv::Mat input_img_pad = cv::Mat::zeros(
      input_img.rows + Q_K - 1, input_img.cols + Q_K - 1, input_img.type());
  cv::Mat roi_input_img_pad =
      input_img_pad(cv::Rect{offset, offset, input_img.cols, input_img.rows});
  input_img.copyTo(roi_input_img_pad);

  const int step_x = kOutputSize;
  const int step_y = kRowsOutput;

  output_hw = cv::Mat::zeros(input_img.size(), input_img.type());
  std::cout << "Image size: " << output_hw.size() << std::endl;

  /* -- Cycle the Accelerator -- */
  for (int i{0}; i < input_img.rows; i += step_y) {
    for (int j{0}; j < input_img.cols; j += step_x) {
      /* Get the ROI and load the data */
      cv::Rect roi_in{j, i, kWindowSize, kRowsWindow};
      CopyMatToArray(input_img_pad, input_batch[0], roi_in);

      /* Process matrix */
      if (kEnablePrinting) {
        ama::utils::print_matrix<DataType, kRowsWindow, kWindowSize>(
            input_batch);
      }

      /* Upload data */
      UploadImage(input_batch, stream_input);

      /* Run execute */
      WRITE_EXE_PARAM_TB(execution_mode,
                         static_cast<int>(ExecutionModes::EXECUTE));
      EXECUTE_ACCEL;

      /* Download result */
      DownloadImage(stream_output, output_batch);

      if (kEnablePrinting) {
        ama::utils::print_matrix<DataType, kRowsOutput, kOutputSize>(
            output_batch);
      }

      /* Process results */
      cv::Rect roi_out{j, i, step_x, step_y};
      CopyMatFromArray(output_batch[0], output_hw, roi_out);
    }
  }

  /* -------------------------------------------------------- */
  /* --------------------- Software Run --------------------- */
  /* -------------------------------------------------------- */
  /* Compute the SW version */
  CvFilter(input_img, kernel_sw, output_sw);

  std::cout << "Finished" << std::endl;

  /* -------------------------------------------------------- */
  /* --------------------- Save results --------------------- */
  /* -------------------------------------------------------- */

  /* Print output matrices */
  if (argc < 3) {
    cv::imwrite(kDefaultOutputImage, output_hw);
    cv::imwrite(kDefaultOutputImage + "-sw.png", output_sw);
  } else {
    cv::imwrite(argv[2], output_hw);
    cv::imwrite(std::string{argv[2]} + "-sw.png", output_sw);
  }

  /* -------------------------------------------------------- */
  /* ----------------- Testbench Comparison ----------------- */
  /* -------------------------------------------------------- */

  cv::Mat output_float_sw, output_float_hw;
  output_sw.convertTo(output_float_sw, CV_64F);
  output_hw.convertTo(output_float_hw, CV_64F);

  cv::Mat abs_difference = (cv::abs(output_float_sw - output_float_hw) / 256.);
  auto mean_std = ama::utils::mean_std(abs_difference);

  std::cout << "Image RMSE: "
            << std::sqrt(ama::utils::mse(output_float_sw, output_float_hw)) /
                   256.
            << std::endl;
  std::cout << "Image PSNR: "
            << ama::utils::psnr(output_float_sw, output_float_hw) << std::endl;
  std::cout << "Image Mean: " << mean_std.first << std::endl;
  std::cout << "Image Std: " << mean_std.second << std::endl;
  std::cout << "Image SSIM: "
            << ama::utils::ssim(output_float_sw, output_float_hw) << std::endl;

  /* Co-sim patch */
  WRITE_EXE_PARAM_TB(execution_mode, static_cast<int>(ExecutionModes::NOP));
  EXECUTE_ACCEL;

  return ret;
}
