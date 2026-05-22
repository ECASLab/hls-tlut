# 1. Buscamos el compilador para poder encontrar las librerías de Xilinx
cpp = meson.get_compiler('cpp')

xrt_inc = include_directories('/opt/xilinx/xrt/include')
xrt_lib_dir = '/opt/xilinx/xrt/lib'

xrt_core_lib = cpp.find_library('xrt_core', dirs: xrt_lib_dir)
xrt_coreutil_lib = cpp.find_library('xrt_coreutil', dirs: xrt_lib_dir)
thread_dep = dependency('threads')

xrt_dep = declare_dependency(
    dependencies: [xrt_core_lib, xrt_coreutil_lib, thread_dep],
    include_directories: xrt_inc
)

# 2. Tu configuración actual del LeNet5
model_source = [
    'main.cpp',
    '../../../api/tlut_api.cpp'
]

model_inc = ['.']

executable ('mqlenet5',
    model_source,
    cpp_args : model_args,
    install : false,
    include_directories : [projectinc, include_directories('../../../api')],
    dependencies : [project_deps, libmodelexecuter_dep, xrt_dep] # <--- Agregamos xrt_dep aquí
)