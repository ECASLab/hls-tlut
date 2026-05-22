/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

#include <cmath>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "error-metrics.hpp"

namespace fal {
namespace profile {

/**
 * Error profiler class
 * This class is able to compute several error metrics: mean, std, max, min,
 * mse, rmse, PSNR, SSIM, Frobenius, Histograms...
 * <br>
 * <b>Example</b>
 * @code
 * cv::Mat ref, exp;
 * // ...
 * Profiler prof{};
 * prof.Register(ref, exp, 256.);
 * prof.Print();
 * @endcode
 */
class Profiler {
 private:
  /** Mask that sets which metrics to compute */
  const int measure_mask_;
  /** Number of bins for the histogram */
  const int hist_bins_;
  /** Maximum value of the histogram */
  const double hist_max_;

  /** Print the values flag */
  bool printed_;

  /** Absolute mean register */
  double norm_abs_mean_;
  /** Absolute std register */
  double norm_abs_std_;
  /** Absolute min register */
  double norm_abs_min_;
  /** Absolute max register */
  double norm_abs_max_;
  /** MSE register */
  double mse_;
  /** RMSE register */
  double rmse_;
  /** PSNR register */
  double psnr_;
  /** SSIM register */
  double ssim_;
  /** Frobenius distance register */
  double frob_distance_;

  /** Error histogram register */
  cv::Mat error_hist_;

  /* File writer */
  std::ofstream error_file_;

  /* Local Matrices */
  cv::Mat img_ref_;
  cv::Mat img_exp_;

 public:
  /**
   * Default constructor. It defines: <br>
   * <ul>
   * <li>Mask = -1: All</li>
   * <li>Number of bins = 50</li>
   * <li>Maximum value of the histogram = 0.5</li>
   * </ul>
   */
  Profiler()
      : measure_mask_{-1},
        hist_bins_{50},
        hist_max_{0.5},
        printed_{false},
        error_file_{"error-results.log"} {}

  /**
   * Constructor with personalised mask. It defines: <br>
   * @param measure_mask operations mask
   * <ul>
   * <li>Number of bins = 50</li>
   * <li>Maximum value of the histogram = 0.5</li>
   * </ul>
   */
  explicit Profiler(const int measure_mask)
      : measure_mask_{measure_mask},
        hist_bins_{20 * 5},
        hist_max_{5},
        printed_{false},
        error_file_{"error-results.log"} {}

  /**
   * Constructor with personalised mask. It defines: <br>
   * @param measure_mask operations mask
   * @param hist_bins number of bins of the histogram
   * @param hist_max maximum value of the histogram
   */
  Profiler(const int measure_mask, const int hist_bins, const double hist_max)
      : measure_mask_{measure_mask},
        hist_bins_{hist_bins},
        hist_max_{hist_max},
        printed_{false},
        error_file_{"error-results.log"} {}

  /**
   * Registers two matrices and compute the error metrics
   * @param img_exp experimental matrix (approximate)
   * @param img_ref reference matrix (reference)
   * @param normalisation normalisation of the elements. Defaults to 1.
   */
  void Register(const cv::Mat& img_exp, const cv::Mat& img_ref,
                const double normalisation);

  /**
   * Registers two matrices and compute the error metrics
   * @param exp experimental matrix (approximate)
   * @param ref reference matrix (reference)
   * @param rows number of rows of the matrices
   * @param cols number of columns of the matrices
   * @param normalisation normalisation of the elements. Defaults to 1.
   */
  void Register(float* exp, float* ref, const int rows, const int cols,
                const double normalisation);

  /**
   * Print the metrics
   * <b>Example</b>
   * @code
   * -- Printing results --
   * Mean: 0.00778491
   * Std: 0.00193784
   * @endcode
   */
  void Print();

  /**
   * Default destructor: if #Profiler::Print() has not been invoked,
   * it is invoked on destruction
   */
  ~Profiler() {
    if (!printed_) Print();
    error_file_.close();
  }

  /**
   * Types of metrics for the mask (bit position)
   */
  enum {
    /** Compute the absolute mean and std */
    ABS_MEAN_STD = 0,
    /** Compute the absolute min and max */
    ABS_MIN_MAX,
    /** Compute the MSE */
    MSE,
    /** Compute the RMSE */
    RMSE,
    /** Compute the PSNR */
    PSNR,
    /** Compute the SSIM */
    SSIM,
    /** Compute the Frobenius distance */
    FROBENIUS,
    /** Compute the normalised error histogram */
    HISTOGRAM
  };
};

inline void Profiler::Register(float* exp, float* ref, const int rows,
                               const int cols,
                               const double normalisation = 1.) {
  cv::Mat img_exp(rows, cols, CV_32F, reinterpret_cast<void*>(exp));
  cv::Mat img_ref(rows, cols, CV_32F, reinterpret_cast<void*>(ref));
  Register(img_exp, img_ref, normalisation);
}

inline void Profiler::Register(const cv::Mat& img_exp, const cv::Mat& img_ref,
                               const double normalisation = 1.) {
  
  if (img_exp_.empty() || img_ref_.empty()) {
    img_exp_ = img_exp.clone();
    img_ref_ = img_ref.clone();
  } else {
    cv::Mat img_exp_tmp = img_exp_.clone();
    cv::Mat img_ref_tmp = img_ref_.clone();
    /* Concat */
    cv::hconcat(img_exp_tmp, img_exp, img_exp_);
    cv::hconcat(img_ref_tmp, img_ref, img_ref_);
  }

  cv::Mat abs_difference = (cv::abs(img_ref_ - img_exp_) / normalisation);

  /* Compute the mean normalised error value and std */
  if (measure_mask_ & (0x1 << ABS_MEAN_STD)) {
    auto mean_std = fal::profiler::metrics::MeanStd(abs_difference);
    norm_abs_mean_ = mean_std.first;
    norm_abs_std_ = mean_std.second;
  }

  /* Compute the min max of the normalised error value */
  if (measure_mask_ & (0x1 << ABS_MIN_MAX)) {
    auto min_max = fal::profiler::metrics::MinMax(abs_difference);
    norm_abs_min_ = min_max.first;
    norm_abs_max_ = min_max.second;
  }

  /* Compute MSE and RMSE */
  if (measure_mask_ & (0x1 << MSE)) {
    mse_ = fal::profiler::metrics::MSE(img_ref_, img_exp_);
  }
  if (measure_mask_ & (0x1 << RMSE)) {
    rmse_ = std::sqrt(mse_);
  }

  /* Compute PSNR */
  if (measure_mask_ & (0x1 << PSNR)) {
    psnr_ = fal::profiler::metrics::PSNR(img_ref_, img_exp_);
  }

  /* Compute SSIM */
  if (measure_mask_ & (0x1 << SSIM)) {
    ssim_ = fal::profiler::metrics::SSIM(img_ref_, img_exp_);
  }

  /* Compute histograms*/
  if (measure_mask_ & (0x1 << HISTOGRAM)) {
    error_hist_ = fal::profiler::metrics::Histogram(abs_difference, hist_bins_,
                                                    hist_max_);
  }

  /* Compute the Frobenius distance */
  if (measure_mask_ & (0x1 << FROBENIUS)) {
    frob_distance_ =
        fal::profiler::metrics::FrobeniusDistance(img_ref_, img_exp_);
  }

  /* Print the difference matrix */
  cv::Mat abs_difference_float;
  abs_difference.convertTo(abs_difference_float, CV_32F);

  cv::Mat abs_difference_float_1d =
      cv::Mat(1, abs_difference_float.cols * abs_difference_float.rows,
              CV_32FC1, abs_difference_float.data);

  error_file_ << abs_difference_float_1d;
}

inline void Profiler::Print() {
  std::cout << "-- Printing results --" << std::endl;
  if (measure_mask_ & (0x1 << ABS_MEAN_STD)) {
    std::cout << "Mean: " << norm_abs_mean_ << std::endl
              << "Std: " << norm_abs_std_ << std::endl;
  }
  if (measure_mask_ & (0x1 << ABS_MIN_MAX)) {
    std::cout << "Min: " << norm_abs_min_ << std::endl
              << "Max: " << norm_abs_max_ << std::endl;
  }
  if (measure_mask_ & (0x1 << MSE)) {
    std::cout << "MSE: " << mse_ << std::endl;
  }
  if (measure_mask_ & (0x1 << PSNR)) {
    std::cout << "PSNR: " << psnr_ << std::endl;
  }
  if (measure_mask_ & (0x1 << SSIM)) {
    std::cout << "SSIM: " << ssim_ << std::endl;
  }
  if (measure_mask_ & (0x1 << HISTOGRAM)) {
    std::cout << "Histogram: " << error_hist_ << std::endl;
    std::cout << "HistMin: " << 0 << std::endl;
    std::cout << "HistMax: " << hist_max_ << std::endl;
    std::cout << "HistBins: " << hist_bins_ << std::endl;
  }
  if (measure_mask_ & (0x1 << FROBENIUS)) {
    std::cout << "Frobenius: " << frob_distance_ << std::endl;
  }
  printed_ = true;
}

} /* namespace profile */
} /* namespace fal */
