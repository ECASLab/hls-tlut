# Copyright 2022
# Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>

project('approximate-math-operators-simple-consumer', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3'])

# Dependencies
axc_math_proj = subproject('approximate-math-operators')
axc_math_dep = axc_math_proj.get_variable('axc_math_operators_dep')

axc_test_exe = executable('simple-consumer', ['simple-consumer.cpp'], dependencies : [axc_math_dep])
