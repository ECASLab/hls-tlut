model_args = cpp_args
model_args += [
    '-DQ_DROP=' + get_option('axc-drop').to_string(),
    '-DQ_LOSS_MULT=1',
    '-DQ_LOSS_ADD=1',
    '-DQ_INT=' + get_option('axc-integer').to_string(),
    '-DQ_BW=' + get_option('axc-width').to_string(),
    '-DQ_DENSE_INT=' + get_option('axc-integer').to_string(),
    '-DQ_DENSE_BW=' + get_option('axc-width').to_string(),
    '-DQ_CONV_INT=4',
    '-DQ_CONV_BW=12',
    '-DQ_ADD1D_INT=' + get_option('axc-integer').to_string(),
    '-DQ_ADD1D_BW=' + get_option('axc-width').to_string(),
    '-DQ_ADD2D_INT=4',
    '-DQ_ADD2D_BW=12',
    '-DEXACT=0',
    '-DLSBDROP=1',
    '-DLSBOR=2',
    '-DLSBFIXED=3',
    '-DAK_METHOD=' + get_option('arithmetic-mode'),
    '-DEXTERNAL_BW=' + get_option('axc-external').to_string(),
    '-DMATRIX_MULTIPLIER=' + get_option('matrix-multiplier'),
    '-DINTERNAL_BW=' + get_option('axc-internal').to_string(),
    '-DACCELERATION=ACC_' + get_option('accelerators').to_upper(),
    '-DACC_NONE=0',
    '-DACC_EMULATED=1',
]

subdir('lenet5')
subdir('benchmark-lenet5')
subdir('qlenet5')
subdir('mqlenet5')
subdir('axc-qlenet5')
subdir('axc-tiny-ad08')
subdir('warp-perspective')
subdir('fft-1D')
subdir('axc-mobilenetv2')
