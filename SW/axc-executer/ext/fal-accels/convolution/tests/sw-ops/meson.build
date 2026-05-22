# Add OpenCV dependency
opencv_dep = dependency('opencv4')

convolution_src = ['convolution.cpp']
open_src = ['open.cpp']
convFFT_src = ['convFFT.cpp']
# For new sources:
# replicate _src

executable('convolution', convolution_src, include_directories : include_dir, dependencies: [opencv_dep])
executable('open', open_src, include_directories : include_dir, dependencies: [opencv_dep])
executable('convFFT', convFFT_src, include_directories : include_dir, dependencies: [opencv_dep])
# For new sources:
# replicate executable
