/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

/* Enumerators */
enum class ExecutionModes {
  NOP = 0,
  CONFIG,
  LOAD_KERNEL,
  LOAD_DATA,     /* Deprecated */
  RETRIEVE_DATA, /* Deprecated */
  EXECUTE,
};
enum class Datatypes {
  INT = 0,
  FLOAT = 2,
  FXP_NORM = 3,
  FXP = 4,
};
enum class PaddingType {
  NONE = 0,
  SAME = 1,
};
