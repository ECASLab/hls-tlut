# Copyright 2021-2022
# Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>

project('approximate-math-operators', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3'])

# Include dir
include_dir = include_directories('axc-math', 'ext/hls-types/include')
install_subdir('axc-math', install_dir : 'include')

# Declare dependencies
deps = []
axc_math_operators_dep = declare_dependency(
  compile_args: [],
  dependencies: deps,
  include_directories: include_dir)

# Testing suite
if get_option('build_tests')
  gtest_proj = subproject('gtest')
  gtest_dep = gtest_proj.get_variable('gtest_dep')
  message('Building Tests')
  subdir('tests')
endif

if get_option('build_examples')
  message('Building Examples')
  subdir('samples')
endif
