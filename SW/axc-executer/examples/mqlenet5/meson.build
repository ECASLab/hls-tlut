model_source = [
    'main.cpp',
    '../../../api/tlut_api.cpp'
]

model_inc = ['.']

executable ('mqlenet5',
    model_source,
    cpp_args : model_args,
    install : false,
    include_directories : [projectinc, include_directories('../../api')],
    dependencies : [project_deps, libmodelexecuter_dep]
)