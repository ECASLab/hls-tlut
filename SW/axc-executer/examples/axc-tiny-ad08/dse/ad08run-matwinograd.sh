#!/bin/bash

cd ../../..
#./builddir/examples/axc-tiny-ad08/axc-ad08 ./examples/axc-tiny-ad08/weights  ./examples/axc-tiny-ad08/test/x_test_0_614_196_64.bin 1 y_pred_fal_approx_matfma_3bits_0_614.bin
meson setup builds/builddir-winograd -Dmatrix-multiplier=MatrixMultiplyAddWinograd
ninja -C builds/builddir-winograd
./builds/builddir-winograd/examples/axc-tiny-ad08/axc-ad08 ./examples/axc-tiny-ad08/weights  ./examples/axc-tiny-ad08/test/x_test_0_614_196_64.bin 614 ./builds/y_pred_winograd_0_614.bin
./builds/builddir-winograd/examples/axc-tiny-ad08/axc-ad08 ./examples/axc-tiny-ad08/weights  ./examples/axc-tiny-ad08/test/x_test_1_615_196_64.bin 615 ./builds/y_pred_winograd_1_615.bin
./builds/builddir-winograd/examples/axc-tiny-ad08/axc-ad08 ./examples/axc-tiny-ad08/weights  ./examples/axc-tiny-ad08/test/x_test_2_615_196_64.bin 615 ./builds/y_pred_winograd_2_615.bin
./builds/builddir-winograd/examples/axc-tiny-ad08/axc-ad08 ./examples/axc-tiny-ad08/weights  ./examples/axc-tiny-ad08/test/x_test_3_615_196_64.bin 615 ./builds/y_pred_winograd_3_615.bin