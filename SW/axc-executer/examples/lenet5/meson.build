model_source = [
    'main.cpp'
]

subdir('model')

model_inc = ['.']

executable ('lenet5',
    model_source,
    cpp_args : cpp_args,
    install : false,
    include_directories : [projectinc],
    dependencies : [project_deps, libmodelexecuter_dep]
)

