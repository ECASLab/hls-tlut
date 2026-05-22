model_source = [
    'main.cpp',
]

model_inc = ['.']

executable ('axc-ad08',
    model_source,
    cpp_args : model_args,
    install : false,
    include_directories : [projectinc],
    dependencies : [project_deps, libmodelexecuter_dep]
)

