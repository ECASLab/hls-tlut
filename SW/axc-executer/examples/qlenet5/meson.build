model_source = [
    'main.cpp'
]

model_inc = ['.']

executable ('qlenet5',
    model_source,
    cpp_args : model_args,
    install : false,
    include_directories : [projectinc],
    dependencies : [project_deps, libmodelexecuter_dep]
)

