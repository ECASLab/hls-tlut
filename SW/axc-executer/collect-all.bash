#!/bin/bash

LSB_BITS="1 2 3 4"
LSB_TECHS="LSBDROP LSBOR"
INT_BITS="5 6 7"
BW_BITS="12 14 16"
SAMPLES=5000
TOTAL=72
SAMPLE=1

BIN=examples/benchmark/benchmark-lenet5
MODEL=examples/lenet5/model-weights
INPUTS=examples/benchmark/model-tests/mnist-input-10000.bin
GTRUTH=examples/benchmark/model-tests/mnist-output-10000.bin

for bwbits in $BW_BITS; do
for intbits in $INT_BITS; do

    # Run agnostic part: exact and quantised
    echo "Sample: $SAMPLE / $TOTAL -> Int bits: ${intbits}, BW bits: ${bwbits} in quantised and exact mode"
    M_BUILD_PATH=builddir-${bwbits}-${intbits}-na-na
    meson ${M_BUILD_PATH}               \
        -Darithmetic-mode=EXACT         \
        -Daxc-integer=${intbits}        \
        -Daxc-width=${bwbits} &> data/${M_BUILD_PATH}.meson.log
    ninja -C ${M_BUILD_PATH} &> data/${M_BUILD_PATH}.ninja.log
    ${M_BUILD_PATH}/${BIN} ${MODEL} ${INPUTS} ${SAMPLES} ${GTRUTH} 0 2> data/${M_BUILD_PATH}-float.err.log 1> data/${M_BUILD_PATH}-float.out.log
    ${M_BUILD_PATH}/${BIN} ${MODEL} ${INPUTS} ${SAMPLES} ${GTRUTH} 1 2> data/${M_BUILD_PATH}-quantised.err.log 1> data/${M_BUILD_PATH}-quantised.out.log

for lsbtechs in $LSB_TECHS; do
for lsbbits in $LSB_BITS; do

    # Run aproximate part 
    echo "Sample: $SAMPLE / $TOTAL -> LSB Bits: ${lsbbits}, LSB Techs: ${lsbtechs}, Int bits: ${intbits}, BW bits: ${bwbits}"
    M_BUILD_PATH=builddir-${bwbits}-${intbits}-${lsbtechs}-${lsbbits}
    meson ${M_BUILD_PATH}               \
        -Darithmetic-mode=${lsbtechs}   \
        -Daxc-integer=${intbits}        \
        -Daxc-width=${bwbits}           \
        -Daxc-drop=${lsbbits} &> data/${M_BUILD_PATH}.meson.log
    ninja -C ${M_BUILD_PATH} &> data/${M_BUILD_PATH}.ninja.log
    ${M_BUILD_PATH}/${BIN} ${MODEL} ${INPUTS} ${SAMPLES} ${GTRUTH} 2 2> data/${M_BUILD_PATH}-approx.err.log 1> data/${M_BUILD_PATH}-approx.out.log
    SAMPLE=`expr $SAMPLE + 1`
done
done
done
done