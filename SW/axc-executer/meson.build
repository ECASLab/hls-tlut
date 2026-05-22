project('model-executer',
        ['cpp'],
	version : '0.2.0',
	meson_version : '>= 0.58',
    default_options : ['cpp_std=c++17'])

project_name = meson.project_name()
project_version = meson.project_version()
version_arr = project_version.split('.')
version_major = version_arr[0].to_int()
version_minor = version_arr[1].to_int()
version_micro = version_arr[2].to_int()
if version_arr.length() == 4
  version_nano = version_arr[3].to_int()
else
  version_nano = 0
endif
version_is_dev = version_minor % 2 == 1 and version_micro < 90

# Get an object returns describing a compiler
cc = meson.get_compiler('cpp')
cpp_args = [
   '-Ofast',
   '-march=native',
   '-fno-signed-zeros',
   '-fno-trapping-math',
   '-fassociative-math',
   '-freciprocal-math',
   '-frename-registers',
   '-funroll-loops',
]

# Dependencies
project_deps = []
omp = dependency('openmp', required: false)
if omp.found()
  project_deps += omp
  cpp_args += '-DWITH_OMP'
endif
project_deps += cc.find_library('stdc++fs')

# Define header directories
incdirs = [
  '.',
  'include'
]
subdir('ext')
projectinc = include_directories(incdirs)

# Verify if the warning flags are available in the compiler
# If the flags is availale for the compiler it wiil be used in all compiler
# invocations with the exception of compile tests.
warning_flags = [
  '-Wmissing-declarations',
  '-Wredundant-decls',
  '-Wundef',
  '-Wwrite-strings',
  '-Wformat',
  '-Wformat-nonliteral',
  '-Wformat-security',
  '-Winit-self',
  '-Wmissing-include-dirs',
  '-Waddress',
  '-Wno-multichar',
  '-Wvla',
  '-Wpointer-arith',
]

add_project_arguments(cc.get_supported_arguments(warning_flags), language: 'cpp')

# Install pre-commit hooks
developer_mode = get_option('developer-mode')
if developer_mode
  pre_commit = find_program('pre-commit', required: true, native: true)
  run_command(pre_commit, 'install')
  run_command(pre_commit, 'install', '--hook-type', 'commit-msg')
endif

debug_mode = get_option('debug-mode')
if debug_mode
  cpp_args += '-DDEBUG_MODE'
endif

# Create an empty configuration object to set config.h information
cdata = configuration_data()

subdir('src') 
subdir('examples')
