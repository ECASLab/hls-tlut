sources = [
  'data.cpp' ,
  'executer.cpp',
  'model.cpp' ,
  ]

libmodelexecuter = static_library('libmodelexecuter' ,
  sources, 
  cpp_args : cpp_args,
  install : true,
  include_directories : [projectinc],
  dependencies : [project_deps]
)

libmodelexecuter_dep = declare_dependency(link_with: libmodelexecuter)
