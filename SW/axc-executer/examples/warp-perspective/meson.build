# Add OpenCV dependency
opencv_dep = dependency('opencv4', required: false)

warp_perspective_source = [
  'main.cpp'
]

model_inc = ['.']

if opencv_dep.found()
  executable ('warp-perspective',
    warp_perspective_source,
    cpp_args : model_args,
    install : false,
    include_directories : [projectinc],
    dependencies : [project_deps, opencv_dep, libmodelexecuter_dep]
  )
endif
