# Copyright 2021
# Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>

exact_src = [
  'exact.cpp',
  'gtest.cpp'
]

exact_exe = executable(
  'exact',
  exact_src,
  dependencies : [gtest_dep],
  include_directories: include_dir
)

test('exact', exact_exe)
