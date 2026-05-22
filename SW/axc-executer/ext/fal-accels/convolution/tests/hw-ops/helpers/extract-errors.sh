#!/bin/bash
############################################################
## Copyright 2022
## Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
############################################################

cd measurements

PROC_FOLDER=processed
mkdir ${PROC_FOLDER}

for file in $(ls *.log*);
do
  # Get run info
  ACCEL=$(echo ${file} | awk -F  "_" '{print $6}' | awk -F  "." '{print $1}')
  Q_KS=$(echo ${file} | awk -F  "_" '{print $3}')
  Q_BW=$(echo ${file} | awk -F  "_" '{print $4}')
  Q_O=$(echo ${file} | awk -F  "_" '{print $5}')

  # Get image file
  IMG=$(cat ${file} | grep -e "Image:" | awk '{print $2}' | awk -F "/" '{print $(NF)}' | awk -F  "." '{print $1}')

  # Get metrics img
  IMG_RMSE=$(cat ${file} | grep -e "Image RMSE" | awk '{print $3}')
  IMG_PSNR=$(cat ${file} | grep -e "Image PSNR" | awk '{print $3}')
  IMG_MEAN=$(cat ${file} | grep -e "Image Mean" | awk '{print $3}')
  IMG_STD=$(cat ${file} | grep -e "Image Std" | awk '{print $3}')
  IMG_SSIM=$(cat ${file} | grep -e "Image SSIM" | awk '{print $3}')
  
  # Get metrics Kernel
  KER_RMSE=$(cat ${file} | grep -e "Kernel RMSE" | awk '{print $3}')
  KER_PSNR=$(cat ${file} | grep -e "Kernel PSNR" | awk '{print $3}')
  KER_MEAN=$(cat ${file} | grep -e "Kernel Mean" | awk '{print $3}')
  KER_STD=$(cat ${file} | grep -e "Kernel Std" | awk '{print $3}')

  # Get histogram
  HIST_BINS=$(cat ${file} | grep -e "Hist" | awk '{print $2}')
  HIST_PERC=$(cat ${file} | grep -e "Hist" | awk '{print $4}' | sed -e "s/://g")
  HIST_DATA=$(cat ${file} | grep -e "Hist" | awk -F "[" '{print $2}' | sed -e "s/; /\n/g" -e "s/\[//g" -e "s/\]//g")

  # Export histogram
  if [ "$Q_O" == "2" ]; then
    echo "${HIST_DATA}" > ${PROC_FOLDER}/hist_${HIST_BINS}_${HIST_PERC}_${IMG}_${Q_BW}_${ACCEL}_${Q_KS}.data
  fi
  # Echo the error
  echo "${Q_KS} ${Q_BW} ${Q_O} ${IMG_RMSE} ${IMG_PSNR} ${IMG_MEAN} ${IMG_STD} ${IMG_SSIM}" >> ${PROC_FOLDER}/stats_${ACCEL}_${IMG}.data
  echo "${Q_KS} ${Q_BW} ${Q_O} ${KER_PSNR} ${KER_MEAN} ${KER_STD}" >> ${PROC_FOLDER}/stats_${ACCEL}_kernel.data
done

# Process plots
cd ${PROC_FOLDER}
bash datatypes-histogram.sh
bash datatypes-metrics.sh
bash consumptions.sh
