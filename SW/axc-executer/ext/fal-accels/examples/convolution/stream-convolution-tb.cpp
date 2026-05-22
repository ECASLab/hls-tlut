/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

/**
 * @example stream-convolution-tb.cpp
 * This is the main testbench implementation of the convolution accelerator
 */

#include "stream-convolution-tb.hpp"

#include <iostream>
#include <opencv2/opencv.hpp>

#include "profile/degreesoffreedom.hpp"
#include "profile/profiler.hpp"
#include "stream-convolution.hpp"

#define ANCHOR cv::Point(-1, -1)
#define BORDER_TYPE cv::BORDER_CONSTANT
#define DELTA 0
#define IMAGE_DEPTH CV_64F

/**
 * Get a new Gaussian Filter
 * @param kernel_size side size of the kernel
 * @returns the cv::Mat with the required kernel
 */
static cv::Mat GetFilter(const int kernel_size) {
  cv::Mat filter;
  double sigma = sigma = (kernel_size + 2) / 6;

  filter = cv::getGaussianKernel(kernel_size, sigma, IMAGE_DEPTH);
  return filter * filter.t();
}

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
static void DownloadImage(StreamPort &stream_output,
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

template <typename T = uint8_t, bool norm = true, typename TO>
static void CopyMatToArray(const cv::Mat &input, TO *output,
                           const cv::Rect &roi) {
  for (int i{0}; i < roi.height; ++i) {
    for (int j{0}; j < roi.width; ++j) {
      float val = input.at<T>(i + roi.y, j + roi.x);
      output[i * roi.width + j] = norm ? (val / 256.f - 0.5f) : val;
    }
  }
}

template <typename T = uint8_t, bool norm = true, typename TO>
static void CopyMatFromArray(const TO *input, cv::Mat &output,
                             const cv::Rect &roi) {
  for (int i{0}; i < roi.height; ++i) {
    for (int j{0}; j < roi.width; ++j) {
      float val = input[i * roi.width + j];
      output.at<T>(roi.y + i, j + roi.x) = norm ? ((val + 0.5f) * 256.f) : val;
    }
  }
}

static void CvFilter(const cv::Mat &input, const cv::Mat &kernel,
                     cv::Mat &output) {
  cv::filter2D(input, output, IMAGE_DEPTH, kernel, ANCHOR, DELTA, BORDER_TYPE);
}

int main(int argc, char **argv) {
  const std::string kDefaultInputImage{TOSTRING(CONV_IMAGE)};
  const bool kEnablePrinting = false;
  int execution_counter = 0;

  /* Batch buffers */
  DataType output_batch[kRowsOutput][kOutputSize];
  DataType input_batch[kRowsWindow][kWindowSize];

  /* Kernel buffer */
  DataType kernels[kKernelSize * kKernelSize];

  /* Streaming ports */
  StreamPort stream_input{}, stream_output{};

  /* Get the input image */
  std::string input_img_name{}, output_img_name{};

  /* -- Get image -- */
  cv::Mat input_img, output_hw, output_sw, kernel_sw;

  if (argc == 1) {
    input_img_name = kDefaultInputImage;
    output_img_name = kDefaultInputImage;
  } else if (argc == 2) {
    input_img_name = argv[1];
    output_img_name = input_img_name;
  } else if (argc == 3){
    input_img_name = argv[1];
    output_img_name = argv[2];
  } else {
    std::cerr << "Cannot process the arguments" << std::endl;
    return -1;
  }

  std::cout << "Reading: " << input_img_name << std::endl; 

  input_img = cv::imread(input_img_name, 0);

  /* Check it */
  if (input_img.empty()) {
    std::cerr << "Cannot open the image" << std::endl;
    return -1;
  }

  /* Execution params */
  DECL_EXE_PARAMS_TB;

  /* Capabilities */
  DECL_CAPS_TB;

  /* -- Write properties -- */
  WRITE_EXE_PARAM_TB(input_width, input_img.cols);  /* Unused */
  WRITE_EXE_PARAM_TB(input_height, input_img.rows); /* Unused */
  WRITE_EXE_PARAM_TB(output_width, input_img.cols);
  WRITE_EXE_PARAM_TB(output_height, input_img.rows);
  WRITE_EXE_PARAM_TB(kernel_size, kKernelSize);

  /* -- Execute -- */
  WRITE_EXE_PARAM_TB(execution_mode, static_cast<int>(ExecutionModes::CONFIG));
  EXECUTE_ACCEL;

  /* -- Read properties back -- */
  READ_PROPS;

  /* -- Set kernel -- */
  kernel_sw = GetFilter(kKernelSize);
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
      ++execution_counter;

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

  READ_PROPS;

  std::cout << "HW Simulation finished with input queue: "
            << stream_input.size()
            << " and output queue: " << stream_input.size() << std::endl;

  /* -------------------------------------------------------- */
  /* --------------------- Software Run --------------------- */
  /* -------------------------------------------------------- */
  /* Compute the SW version */
  CvFilter(input_img, kernel_sw, output_sw);

  std::cout << "Finished simulation" << std::endl;

  /* -------------------------------------------------------- */
  /* --------------------- Evaluation ----------------------- */
  /* -------------------------------------------------------- */
  /* DoFs */
  fal::profile::DegreesOfFreedomReg dof{};
  dof.Register("Image name", input_img_name);
  dof.Register("Core", TOSTRING(Q_CORE));
  dof.Register("Image Rows", input_img.rows);
  dof.Register("Image Columns", input_img.cols);
  dof.Register("PEs", Q_PES);
  dof.Register("PE Rows", kOutputSize);
  dof.Register("PE Columns", kOutputSize);
  dof.Register("Kernel Rows", kKernelSize);
  dof.Register("Kernel Columns", kKernelSize);
  dof.Register("Datatype Width", Q_BW);
  dof.Register("Datatype Integer", Q_INT);
  dof.Register("Datatype Fraction", Q_BW - Q_INT);
  dof.Register("Execution Counter", execution_counter);
  dof.Print();

  cv::Mat output_float_sw, output_float_hw;
  output_sw.convertTo(output_float_sw, CV_64F);
  output_hw.convertTo(output_float_hw, CV_64F);
  int error_evaluation = (1 << fal::profile::Profiler::ABS_MEAN_STD) |
                         (1 << fal::profile::Profiler::ABS_MIN_MAX) |
                         (1 << fal::profile::Profiler::MSE) |
                         (1 << fal::profile::Profiler::RMSE) |
                         (1 << fal::profile::Profiler::PSNR) |
                         (1 << fal::profile::Profiler::SSIM) |
                         (1 << fal::profile::Profiler::FROBENIUS) |
                         (1 << fal::profile::Profiler::HISTOGRAM);
  fal::profile::Profiler profiler{error_evaluation, 20, 1.};
  profiler.Register(output_float_hw, output_float_sw, 256.);
  profiler.Print();

  /* -------------------------------------------------------- */
  /* --------------------- Save results --------------------- */
  /* -------------------------------------------------------- */
  std::string prefix = "-" + std::to_string(kKernelSize) + "-" +
                       std::to_string(kOutputSize) + "-" +
                       std::to_string(Q_BW) + "-" + std::to_string(Q_INT) +
                       "-" + std::string{TOSTRING(Q_CORE)};
  std::string out_hw = prefix + "-hw.png";
  std::string out_sw = prefix + "-sw.png";

  /* Print output matrices */
  cv::imwrite(output_img_name + out_hw, output_hw);
  cv::imwrite(output_img_name + out_sw, output_sw);

  std::cout << "Result images saved" << std::endl;

  /* -- Run execute -- */
  EXECUTE_ACCEL;

  std::cout << "Closing" << std::endl;

  return 0;
}
