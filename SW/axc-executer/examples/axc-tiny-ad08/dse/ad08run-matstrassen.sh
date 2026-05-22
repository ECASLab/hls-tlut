#!/bin/bash

cd ../../..
#./builddir/examples/axc-tiny-ad08/axc-ad08 ./examples/axc-tiny-ad08/weights  ./examples/axc-tiny-ad08/test/x_test_0_614_196_64.bin 1 y_pred_fal_approx_matfma_3bits_0_614.bin
meson setup builds/builddir-strassen -Dmatrix-multiplier=MatrixMultiplyAddStrassen
ninja -C builds/builddir-strassen
./builds/builddir-strassen/examples/axc-tiny-ad08/axc-ad08 ./examples/axc-tiny-ad08/weights  ./examples/axc-tiny-ad08/test/x_test_0_614_196_64.bin 614 ./builds/y_pred_strassen_0_614.bin
./builds/builddir-strassen/examples/axc-tiny-ad08/axc-ad08 ./examples/axc-tiny-ad08/weights  ./examples/axc-tiny-ad08/test/x_test_1_615_196_64.bin 615 ./builds/y_pred_strassen_1_615.bin
./builds/builddir-strassen/examples/axc-tiny-ad08/axc-ad08 ./examples/axc-tiny-ad08/weights  ./examples/axc-tiny-ad08/test/x_test_2_615_196_64.bin 615 ./builds/y_pred_strassen_2_615.bin
./builds/builddir-strassen/examples/axc-tiny-ad08/axc-ad08 ./examples/axc-tiny-ad08/weights  ./examples/axc-tiny-ad08/test/x_test_3_615_196_64.bin 615 ./builds/y_pred_strassen_3_615.bin