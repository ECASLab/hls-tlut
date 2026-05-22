/*
 * Copyright (C) 2022
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#include <fstream>
#include <iostream>
#include <sstream>

#include <data.hpp>

DataContainer::~DataContainer() {
  if (data) {
    delete[] data;
    data = nullptr;
  }
}

Runtime DataContainer::WriteToFile(const std::filesystem::path &filename) {
  std::streampos size;
  std::stringstream msg_ss;
  std::string msg;
  std::ofstream file(filename, std::ios::out | std::ios::binary);

  if (!file.is_open()) {
    msg_ss << "Cannot open file: " << filename.relative_path();
    return Runtime{Runtime::INVALID_PARAMETER, msg_ss.str()};
  }

  file.write(reinterpret_cast<char *>(this->GetData()), this->size);
  file.close();

  msg_ss << "Written file: " << filename << " - with: " << this->size
         << " Bytes";

  return Runtime{Runtime::OK, msg_ss.str()};
}

Runtime DataContainer::ReadFromFile(const std::filesystem::path &filename) {
  std::streampos size;
  std::stringstream msg_ss;
  std::string msg;
  std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);

  char *memblock = nullptr;

  if (!file.is_open()) {
    msg_ss << "Cannot open file: " << filename.relative_path();
    return Runtime{Runtime::INVALID_PARAMETER, msg_ss.str()};
  }

  size = file.tellg();

  this->size = 1;
  for (int i = 0; i < this->num_dimensions; ++i) {
    this->size *= this->dimensions[i];
  }
  this->size *= sizeof(float);

  if (static_cast<uint64_t>(size) < this->size) {
    msg_ss << "Sizes mismatch. Expected: " << this->size
           << " Received: " << size;
    return Runtime{Runtime::INCOMPATIBLE_PARAMETER, msg_ss.str()};
  }

  file.seekg(0, std::ios::beg);

  memblock = new char[size];
  file.read(memblock, size);

  this->data = reinterpret_cast<float *>(memblock);

  msg_ss << "Open file: " << filename << " - with: " << size << " Bytes";

  file.close();
  return Runtime{Runtime::OK, msg_ss.str()};
}
