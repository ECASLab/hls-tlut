/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

/**
 * Datatypes supported by FAL Accelerators
 */
enum class Datatypes {
  INT = 0,      /* Integer */
  FLOAT = 2,    /* Floating-point */
  FXP_NORM = 3, /* Normalised Fixed-point */
  FXP = 4,      /* Fixed-point */
};

/**
 * Types of padding
 */
enum class PaddingType {
  NONE = 0,   /* None padding */
  SAME = 1,   /* Repeat the border */
  REFLECT = 2 /* Reflect the borders */
};

/**
 * Execution modes (if supported)
 */
enum class ExecutionModes {
  NOP = 0,
  CONFIG,        /* Configure accel */
  LOAD_KERNEL,   /* Load kernel */
  LOAD_DATA,     /* Load data (H2C) */
  RETRIEVE_DATA, /* Retrieve data (C2H) */
  EXECUTE,       /* Execute the accel */
};

/**
 * Accelerator type
 */
enum class AcceleratorType {
  NONE = 0,        /* Invalid */
  CONVOLUTION = 1, /* Convolution */
  MATRIX = 2,      /* Matrix operator: i.e. mult, add, mac */
  MAP = 3,         /* Matrix map: i.e. tanh */
  DF_BINARY = 4,   /* Dataflow binary: i.e. add */
};

/**
 * Allowed matrix operations by the accelerator
 */
enum class MatOperations {
  ADD = 1 << 0,
  MUL = 1 << 1,
  SUB = 1 << 2,
  DIV = 1 << 3,
  FMA = 1 << 4,
  MAC = 1 << 5,
  RELU = 1 << 6,
  SIGMOID = 1 << 7,
  SOFTMAX = 1 << 8,
  ARCTAN = 1 << 9,
  TANH = 1 << 10
  /* TODO: More to be defined */
};

/**
 * IDs of the matrices
 */
enum class MatIdx { A = 0, B, C, D, INVALID };

/**
 * Matrix masks
 */
enum class MatMasks {
  A = 1 << static_cast<int>(MatIdx::A),
  B = 1 << static_cast<int>(MatIdx::B),
  C = 1 << static_cast<int>(MatIdx::C),
  D = 1 << static_cast<int>(MatIdx::D),
  INVALID = 1 << static_cast<int>(MatIdx::INVALID),
  ALL = 0b111
};
