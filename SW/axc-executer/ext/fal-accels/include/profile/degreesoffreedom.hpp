/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

#include <iostream>
#include <unordered_map>

namespace fal {
namespace profile {

/**
 * Data container for the Degrees of Freedom
 */
struct DegreesOfFreedomNum {
  /** Integer-type number datum */
  int intnum;
  /** String-type datum */
  std::string str;
  /** Float-type number datum */
  float floatnum;
  /** Type of datum hold by the structure */
  int type;

  /**
   * Types of datum supported by the structure
   */
  enum { FLOAT, INT, STRING };

  /**
   * Default constructor
   */
  DegreesOfFreedomNum() = default;

  /**
   * Constructor overload for float DoFs
   * @param input: float input
   */
  DegreesOfFreedomNum(const float input) : floatnum{input}, type{FLOAT} {}

  /**
   * Constructor overload for string DoFs
   * @param input: string input
   */
  DegreesOfFreedomNum(const std::string input) : str{input}, type{STRING} {}

  /**
   * Constructor overload for Input DoFs
   * @param input: string input
   */
  DegreesOfFreedomNum(const int input) : intnum{input}, type{INT} {}

  /**
   * Copy assignment for #DegreesOfFreedomNum Structure
   * @param d: structure to copy
   */
  void operator=(const DegreesOfFreedomNum& d) {
    intnum = d.intnum;
    str = d.str;
    floatnum = d.floatnum;
    type = d.type;
  }

  /**
   * Stream operator
   * @param os input ostream object
   * @param dof structure to print
   * @return output ostream object
   */
  friend std::ostream& operator<<(std::ostream& os,
                                  const DegreesOfFreedomNum& dof);
};

std::ostream& operator<<(std::ostream& os, const DegreesOfFreedomNum& dof) {
  std::string s;
  switch (dof.type) {
    case dof.FLOAT:
      s = std::to_string(dof.floatnum);
      break;
    case dof.STRING:
      s = dof.str;
      break;
    case dof.INT:
      s = std::to_string(dof.intnum);
      break;
    default:
      break;
  }
  return os << s;
}

/**
 * Degrees of Freedom Register Class
 * This registers the degrees of freedom, stores them and prints it
 * with format
 * <br>
 * <b>Example</b>
 * @code
 * DegreesOfFreedomReg dof{};
 * dof.Register("Image Columns", 512);
 * dof.Register("Image Rows", 512);
 * dof.Print();
 * @endcode
 */
class DegreesOfFreedomReg {
 private:
  /** Map that holds DoFs */
  std::unordered_map<std::string, DegreesOfFreedomNum> map_;
  /** Print the values flag */
  bool printed_;

 public:
  /**
   * Default constructor
   */
  DegreesOfFreedomReg() : printed_{false} {}

  /**
   * Default destructor: if #DegreesOfFreedomReg::Print() has not been invoked,
   * it is invoked on destruction
   */
  ~DegreesOfFreedomReg() {
    if (!printed_) Print();
  }

  /**
   * Print the degrees of freedom
   * <b>Example</b>
   * @code
   * -- Printing DoFs --
   * Image Columns: 512
   * Image Rows: 512
   * @endcode
   */
  void Print();

  /**
   * Register a new degree of freedom with its value
   * @tparam type of the value: it can be float, integer, string
   * @param key name of the DoF
   * @param value value of the DoF
   * <b>Example</b>
   * @code
   * DegreesOfFreedomReg dof{};
   * dof.Register("Image Columns", 512);
   * dof.Register("Image Rows", 512);
   * dof.Print();
   * @endcode
   */
  template <typename T>
  void Register(const std::string key, const T value);
};

inline void DegreesOfFreedomReg::Print() {
  printed_ = true;
  std::cout << "-- Printing DoFs --" << std::endl;
  for (const std::pair<const std::string, DegreesOfFreedomNum>& elem : map_) {
    std::cout << elem.first << ": " << elem.second << std::endl;
  }
}

template <typename T>
inline void DegreesOfFreedomReg::Register(const std::string key,
                                          const T value) {
  map_[key] = DegreesOfFreedomNum{value};
}

} /* namespace profile */
} /* namespace fal */
