#!/bin/bash

cd ../../..
# Check internals
for i in `seq 10 1 20`; do
  meson setup builds/builddir_20_${i} -Daxc-internal=${i} --optimization=3
  ninja -C builds/builddir_20_${i} 
  ./builds/builddir_20_${i}/examples/axc-tiny-ad08/axc-ad08 ./examples/axc-tiny-ad08/weights ./examples/axc-tiny-ad08/test/x_test_0_614_196_64.bin 614 builds/builddir_matmul_20_${i}_0.bin
  ./builds/builddir_20_${i}/examples/axc-tiny-ad08/axc-ad08 ./examples/axc-tiny-ad08/weights ./examples/axc-tiny-ad08/test/x_test_1_615_196_64.bin 615 builds/builddir_matmul_20_${i}_1.bin
  ./builds/builddir_20_${i}/examples/axc-tiny-ad08/axc-ad08 ./examples/axc-tiny-ad08/weights ./examples/axc-tiny-ad08/test/x_test_2_615_196_64.bin 615 builds/builddir_matmul_20_${i}_2.bin
  ./builds/builddir_20_${i}/examples/axc-tiny-ad08/axc-ad08 ./examples/axc-tiny-ad08/weights ./examples/axc-tiny-ad08/test/x_test_3_615_196_64.bin 615 builds/builddir_matmul_20_${i}_3.bin 
done
