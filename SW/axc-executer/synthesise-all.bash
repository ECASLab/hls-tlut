#!/bin/bash

LSB_BITS="1 2 3 4"
LSB_TECHS="LSBDROP LSBOR"
INT_BITS="5 6 7"
BW_BITS="12 14 16"
SAMPLES=5000
TOTAL=72
SAMPLE=1

FLOAT_FILE=data/float.data
QUANT_FILE=data/quantised.data
APPOX_FILE=data/approximate.data

echo "bwbits intbits accuracy" > ${FLOAT_FILE}
echo "bwbits intbits accuracy" > ${QUANT_FILE}
echo "bwbits intbits lsbtechs lsbbits accuracy" > ${APPOX_FILE}

for bwbits in $BW_BITS; do
for intbits in $INT_BITS; do

    # Run agnostic part: exact and quantised
    echo "Sample: $SAMPLE / $TOTAL -> Int bits: ${intbits}, BW bits: ${bwbits} in quantised and exact mode"
    M_BUILD_PATH=builddir-${bwbits}-${intbits}-na-na
    val=$(cat data/${M_BUILD_PATH}-float.out.log | grep "Accuracy" | awk '{print $2}')
    echo "${bwbits} ${intbits} ${val}" >> ${FLOAT_FILE}
    val=$(cat data/${M_BUILD_PATH}-quantised.out.log | grep "Accuracy" | awk '{print $2}')
    echo "${bwbits} ${intbits} ${val}" >> ${QUANT_FILE}

for lsbtechs in $LSB_TECHS; do
for lsbbits in $LSB_BITS; do
    # Run aproximate part 
    echo "Sample: $SAMPLE / $TOTAL -> LSB Bits: ${lsbbits}, LSB Techs: ${lsbtechs}, Int bits: ${intbits}, BW bits: ${bwbits}"
    M_BUILD_PATH=builddir-${bwbits}-${intbits}-${lsbtechs}-${lsbbits}
    val=$(cat data/${M_BUILD_PATH}-approx.out.log | grep "Accuracy" | awk '{print $2}')
    echo "${bwbits} ${intbits} ${lsbtechs} ${lsbbits} ${val}" >> ${APPOX_FILE}
    SAMPLE=`expr $SAMPLE + 1`
done
done
done
done