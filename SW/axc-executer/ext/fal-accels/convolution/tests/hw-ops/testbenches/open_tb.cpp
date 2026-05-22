/*
 * Copyright 2021
 * Author: Alejandro Rodriguez Figueroa <alejandrorf@estudiantec.cr>
 * Supervisor: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#include <iostream>
#include <opencv2/opencv.hpp>

/* Header from tops */
#include "open.hpp"

/*
 * Please, check the readme.
 * To run this testbench, you need to define the arguments where the image are loaded.
 * For example:
 * TB_ARGV="`pwd`/misc/lenna.png `pwd`/misc/lenna-o.png" TEST=open make test
 */

int main(int argc, char** argv) {
  const float a = 2;
  float b = 0;

  if (argc != 3) {
    std::cerr << "Cannot open the file. No enough args. Usage: "
                 "TB_ARGV=\"path/input.img path/output.img\" "
              << std::endl;
    return -1;
  }

  cv::Mat img = cv::imread(argv[1]);

  std::cout << "Before accel, b is: " << b << std::endl;
  open_top_accel(a, b);
  std::cout << "After accel, b is: " << b << std::endl;

  /* Just as a show case on how to use OCV */
  cv::imwrite(argv[2], img);
  return 0;
}
