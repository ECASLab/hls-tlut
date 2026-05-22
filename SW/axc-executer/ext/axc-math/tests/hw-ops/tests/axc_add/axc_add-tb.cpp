/*
 * Copyright 2022
 * Author: David Cordero Chavarría <dacoch215@estudiantec.cr>
 */

#include <ap_fixed.h> /* ap_fixed<N> types */
#include <ap_int.h>   /* ap_[u]int<N> */

#include <chrono>   /* std::chrono */
#include <cstdint>  /* [u]int<N>_t types */
#include <iomanip>  /* std::setfill, std::setw */
#include <iostream> /* std::cout / std::endl */
#include <memory>   /* std::unique_ptr */
#include <random>   /* std::mt19937 */

#include "axc_add.hpp" /* axc_add_accel_top() */

/*
 * Copyright 2021
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

namespace ama {
namespace utils {

class StatsMeter {
 public:
  /**
   * Default constructor
   * Prepares the vector and the counter before the action
   */
  StatsMeter() : values_{}, min_value_{100.}, max_value_{0.}, mean_{0.f} {}

  /**
   * Default destructor
   */
  ~StatsMeter();
  /**
   * Register a new measurement
   * It register the reference value and the measurement value. It computes the
   * error and saves it into the values vector.
   * @tparam T type of the measurement type
   * @param reference reference value take as a theoretical value
   * @param measured measured value take as the approximation value
   * @param normalisation normalisation value
   */
  template <typename T>
  void Register(const double reference, const T measured,
                const double normalisation);

  /**
   * Print the results
   * Computes the statistical values and prints them out through stdout
   */
  void Print() const;

  /**
   * Compute the histogram of an array of differences.
   * The values are ranged from 0 - max and absolutely evaluated.
   * @param bins number of bins of the histogram
   * @param max max number of the histogram
   */
  std::vector<double> compute_histogram(const int bins, const double max) const;

 private:
  std::vector<double> values_;
  double min_value_;
  double max_value_;
  double mean_;
};

template <typename T>
inline void StatsMeter::Register(const double reference, const T measured,
                                 const double normalisation) {
  double difference = std::abs(reference - static_cast<double>(measured)) /
                      std::abs(normalisation);

  /* Pre-compute min, max to avoid as many runs as possible */
  min_value_ = std::min(difference, min_value_);
  max_value_ = std::max(difference, max_value_);

  /* Compute the mean */
  const auto newsize = static_cast<double>(values_.size() + 1);
  double inv_multiplier = 1.0 / newsize;
  mean_ *= (static_cast<double>(values_.size()) * inv_multiplier);
  mean_ += (difference * inv_multiplier);

  /* Emplace values for the std */
  values_.push_back(std::move(difference));
}

inline StatsMeter::~StatsMeter() { Print(); }

inline std::vector<double> StatsMeter::compute_histogram(
    const int bins, const double max) const {
  /* Allocate and initialise histogram */
  std::vector<double> histogram(bins, 0.);

  double quantum = max / static_cast<double>(bins);

  /* Compute the histogram */
  for (const double val : values_) {
    int index = static_cast<int>(std::abs(val) / quantum);
    if (index >= histogram.size()) continue;
    ++histogram.at(index);
  }

  return histogram;
}

inline void StatsMeter::Print() const {
  const int bins = 500;
  const double max_hist = 0.3;
  /* Copy vector */
  decltype(values_) stdvalues(values_);
  decltype(mean_) mean{mean_};
  std::size_t size{values_.size()};
  double inv_size = 1.0 / size;

  /* Prepare lambda for the per-element transformation: (xi - mean)^2 / N */
  auto per_element_transform = [=](double xi) -> double {
    double acc = xi - mean;
    return acc * acc * inv_size;
  };

  /* Compute the std */
  std::transform(values_.begin(), values_.end(), stdvalues.begin(),
                 per_element_transform);

  double variance = std::accumulate(stdvalues.begin(), stdvalues.end(), 0.);
  double stdval = std::sqrt(variance);

  /* Compute histogram */
  std::vector<double> hist = compute_histogram(bins, max_hist);

  for (auto& v : hist) {
    v = inv_size * v;
  }

  std::cout << " ------------------------------------------------ " << std::endl
            << "| Measurement results                            |" << std::endl
            << " ------------------------------------------------ " << std::endl
            << ">> TestMinVal:  " << min_value_ << std::endl
            << ">> TestMaxVal:  " << max_value_ << std::endl
            << ">> TestMeanVal: " << mean_ << std::endl
            << ">> TestStdVal:  " << stdval << std::endl
            << ">> TestVarVal:  " << variance << std::endl;

  /* Print histogram */
  double hoisted = 100.0 / static_cast<double>(bins);
  for (size_t i = 0; i != hist.size(); ++i) {
    std::cout << "from " << i << " to " << hoisted << "%" << std::endl;
  }
  std::cout << "\% per bin: " << hoisted << std::endl;
  std::cout << "Hist " << bins << " bins " << static_cast<int>(max_hist * 100)
            << "%: [";

  for (size_t i = 0; i != hist.size(); ++i) {
    auto const& elem = hist.at(i);
    std::cout << i + 1 << " " << elem << ";";
  }

  std::cout << "]\n";

  std::cout << " ------------------------------------------------ "
            << std::endl;
}
}  // namespace utils
}  // namespace ama

#define S(U) _S(U)
#define _S(U) #U

constexpr static std::size_t elements = Q_O;

int main(int, char**) {
  /*
   * NOTE: scope the two meters differently to force the quantization meter to
   * print second
   */
  ama::utils::StatsMeter meter_quant{};
  {
    ama::utils::StatsMeter meter_approx{};

    std::random_device dev;
    /* seed to initialize std::mt19937 differently across executions */
    std::mt19937::result_type seed =
        dev() ^
        (static_cast<std::mt19937::result_type>(
             std::chrono::duration_cast<std::chrono::seconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count()) +
         static_cast<std::mt19937::result_type>(
             std::chrono::duration_cast<std::chrono::microseconds>(
                 std::chrono::high_resolution_clock::now().time_since_epoch())
                 .count()));
    std::mt19937 gen(seed);
    /*
     * IMPORTANT: representable values on ap_fixed<> depend on Q_INT, with
     * Q_INT = 1, only values from -1 to 0.999969 are representable
     */
    constexpr auto distrib_min = -1.0f;
    constexpr auto distrib_max = 1.0f;
    constexpr auto distrib_range = distrib_max - distrib_min;
    std::uniform_real_distribution<float> distrib(distrib_min, distrib_max);

    std::unique_ptr<float[]> ref_a_arr(new float[elements]);
    std::unique_ptr<float[]> ref_b_arr(new float[elements]);
    std::unique_ptr<float[]> ref_c_arr(new float[elements]);

    std::unique_ptr<DataType[]> axc_a_arr(new DataType[elements]);
    std::unique_ptr<DataType[]> axc_b_arr(new DataType[elements]);
    std::unique_ptr<DataType[]> axc_c_arr(new DataType[elements]);

    for (auto i = 0; i < elements; ++i) {
      float tmp = distrib(gen);
      ref_a_arr[i] = tmp;
      axc_a_arr[i] = DataType(tmp);
    }
    for (auto i = 0; i < elements; ++i) {
      float tmp = distrib(gen);
      ref_b_arr[i] = tmp;
      axc_b_arr[i] = DataType(tmp);
    }
    for (auto i = 0; i < elements; ++i) {
      float const& a = ref_a_arr[i];
      float const& b = ref_b_arr[i];

      auto const& af = axc_a_arr[i];
      auto const& bf = axc_b_arr[i];

      float const c = a + b;
      float cf = (float)af + (float)bf;

      ref_c_arr[i] = c;

      DataType ca;
      axc_add_accel_top(axc_a_arr[i], axc_b_arr[i], ca);
      axc_c_arr[i] = ca;

      meter_quant.Register(c, cf, distrib_range);
      meter_approx.Register(c, (float)ca, distrib_range);
    }
    /* print debug information for log */
    std::cout << "\n\nfor "
              << "\nQ_O: " << Q_O << "\nQ_BW: " << Q_BW << "\nQ_INT: " << Q_INT
              << "\nMETHOD: " << S(METHOD) << "\nAXC_BITS: " << AXC_BITS
              << std::endl;
    std::cout << std::setfill(' ') << std::setw(12) << "axc_a[i]"
              << " * " << std::setfill(' ') << std::setw(12) << "axc_b[i]"
              << " = " << std::setfill(' ') << std::setw(12) << "axc_c[i]"
              << "\t|||" << std::setfill(' ') << std::setw(12) << "ref_a[i]"
              << " * " << std::setfill(' ') << std::setw(12) << "ref_b[i]"
              << " = " << std::setfill(' ') << std::setw(12) << "ref_c[i]"
              << std::endl;
    std::cout << " ------------------------------------------------ "
              << " ------------------------------------------------ "
              << std::endl;
    for (auto i = 0; i < elements; ++i) {
      std::cout << std::setfill(' ') << std::setw(12) << axc_a_arr[i] << " * "
                << std::setfill(' ') << std::setw(12) << axc_b_arr[i] << " = "
                << std::setfill(' ') << std::setw(12) << axc_c_arr[i] << "\t|||"
                << std::setfill(' ') << std::setw(12) << ref_a_arr[i] << " * "
                << std::setfill(' ') << std::setw(12) << ref_b_arr[i] << " = "
                << std::setfill(' ') << std::setw(12) << ref_c_arr[i]
                << std::endl;
    }
  }

  return 0;
}
