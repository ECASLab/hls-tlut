fft_1d_source = [
  'main.cpp',
]

model_inc = ['.']

executable ('fft-1D',
  fft_1d_source,
  cpp_args : model_args,
  install : false,
  include_directories : [projectinc],
  dependencies : [project_deps, libmodelexecuter_dep]
)
