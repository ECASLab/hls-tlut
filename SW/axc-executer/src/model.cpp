/*
 * Copyright (C) 2022
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#include <model.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include <layer.hpp>
#include <runtime.hpp>

Runtime Model::ReadWeights(const std::filesystem::path &filename,
                           BasicLayer *layer) {
  std::streampos size;
  std::stringstream msg_ss;
  std::string msg;
  std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);
  char *memblock = nullptr;

  if (!layer) {
    return Runtime{Runtime::INVALID_PARAMETER, "Layer is nullptr"};
  }

  if (!file.is_open()) {
    msg_ss << "Cannot open file: " << filename.relative_path();
    return Runtime{Runtime::INVALID_PARAMETER, msg_ss.str()};
  }

  size = file.tellg();

  if (static_cast<uint64_t>(size) != layer->size) {
    msg_ss << "Sizes mismatch. Expected: " << layer->size
           << " Received: " << size;
    return Runtime{Runtime::INVALID_PARAMETER, msg_ss.str()};
  }

  file.seekg(0, std::ios::beg);

  memblock = new char[size];
  file.read(memblock, size);

  layer->data = std::shared_ptr<float>(reinterpret_cast<float *>(memblock),
                                       [](float *p) { delete[] p; });

  msg_ss << "Open file: " << filename << " - with: " << size << " Bytes";

  file.close();
  return Runtime{Runtime::OK, msg_ss.str()};
}
