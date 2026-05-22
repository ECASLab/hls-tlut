#!/bin/bash

cd ../../..
# Check externals
for i in `seq 14 1 24`; do
  meson setup builds/builddir_${i}_16 -Daxc-external=${i} --optimization=3
  ninja -C builds/builddir_${i}_16 
  ./builds/builddir_${i}_16/examples/axc-tiny-ad08/axc-ad08 ./examples/axc-tiny-ad08/weights ./examples/axc-tiny-ad08/test/x_test_0_614_196_64.bin 614 builds/builddir_matmul_${i}_16_0.bin
  ./builds/builddir_${i}_16/examples/axc-tiny-ad08/axc-ad08 ./examples/axc-tiny-ad08/weights ./examples/axc-tiny-ad08/test/x_test_1_615_196_64.bin 615 builds/builddir_matmul_${i}_16_1.bin
  ./builds/builddir_${i}_16/examples/axc-tiny-ad08/axc-ad08 ./examples/axc-tiny-ad08/weights ./examples/axc-tiny-ad08/test/x_test_2_615_196_64.bin 615 builds/builddir_matmul_${i}_16_2.bin
  ./builds/builddir_${i}_16/examples/axc-tiny-ad08/axc-ad08 ./examples/axc-tiny-ad08/weights ./examples/axc-tiny-ad08/test/x_test_3_615_196_64.bin 615 builds/builddir_matmul_${i}_16_3.bin 
done
