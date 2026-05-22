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

model_source = [
    'main.cpp',
    '../lenet5/model/lenet.cpp',
    'api/tlut_api.cpp'
]

model_inc = ['.']

e = executable ('benchmark-lenet5',
    model_source,
    cpp_args : model_args,
    install : false,
    include_directories : [projectinc, xrt_inc],
    dependencies : [project_deps, libmodelexecuter_dep, xrt_dep]
)

fmt = '@0@/@1@'
test('Benchmark', 
    e,
    timeout: 1000,
    args: [
        fmt.format(meson.global_source_root(), 'examples/lenet5/model-weights'),
        fmt.format(meson.global_source_root(), 'examples/benchmark/model-tests/mnist-input-10000.bin'),
        '10000',
        fmt.format(meson.global_source_root(), 'examples/benchmark/model-tests/mnist-output-10000.bin'),
    ])