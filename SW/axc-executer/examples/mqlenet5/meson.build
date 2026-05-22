model_source = [
    'main.cpp',
    files('../../api/tlut_api.cpp') # <-- Agregamos el archivo de la API externa
]

model_inc = ['.']

executable ('mqlenet5',
    model_source,
    cpp_args : model_args,
    install : false,
    include_directories : [projectinc, include_directories('../../api')], # <-- Agregamos la ruta a los .hpp
    dependencies : [project_deps, libmodelexecuter_dep]
)