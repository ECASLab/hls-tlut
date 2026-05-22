# 1. Declarar la dependencia de XRT e incluir la fuente de la API (Igual que en los otros proyectos)
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

# 2. Agregar la fuente de la API a la lista de "sources" (Usa los 3 niveles hacia arriba)
model_source = [
    'main.cpp',
    '../../../api/tlut_api.cpp' 
]

model_inc = ['.']

# 3. Inyectar la dependencia de XRT y los includes en el ejecutable
executable ('axc-ad08',
    model_source,
    cpp_args : model_args,
    install : false,
    include_directories : [projectinc, include_directories('../../../api')],
    dependencies : [project_deps, libmodelexecuter_dep, xrt_dep] # <--- xrt_dep agregado aquí
)