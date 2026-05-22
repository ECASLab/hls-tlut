#!/usr/bin/env sh

# this script is used to collect data from Vivado reports, it goes together
# with `collect_data.sh`

set -xe

HAVE_FD=0
command -v fd > /dev/null && HAVE_FD=1
if [ "${HAVE_FD}" = "0" ]; then
  echo "need fd"
  exit 1
fi

REPORTS=$(fd -t f \\.rpt | grep _top_)

# IMPORTANT: ADDITION

echo "${REPORTS}" | grep axc_add | grep -- -8-1- | xargs -r -I {} sh -c "echo {}; sed -n '/Utilization Estimates/,/Detail/p' {}" > add_8_1_utilization_estimates.txt
echo "${REPORTS}" | grep axc_add | grep -- -8-1- | xargs -r -I {} sh -c "echo {}; sed -n '/Latency/,/Detail/p' {}"               > add_8_1_latency_estimates.txt
echo "${REPORTS}" | grep axc_add | grep -- -8-1- | xargs -r -I {} sh -c "echo {}; sed -n '/Timing/,/Latency/p' {}"               > add_8_1_timing_estimates.txt

echo "${REPORTS}" | grep axc_add | grep -- -8-2- | xargs -r -I {} sh -c "echo {}; sed -n '/Utilization Estimates/,/Detail/p' {}" > add_8_2_utilization_estimates.txt
echo "${REPORTS}" | grep axc_add | grep -- -8-2- | xargs -r -I {} sh -c "echo {}; sed -n '/Latency/,/Detail/p' {}"               > add_8_2_latency_estimates.txt
echo "${REPORTS}" | grep axc_add | grep -- -8-2- | xargs -r -I {} sh -c "echo {}; sed -n '/Timing/,/Latency/p' {}"               > add_8_2_timing_estimates.txt

echo "${REPORTS}" | grep axc_add | grep -- -8-4- | xargs -r -I {} sh -c "echo {}; sed -n '/Utilization Estimates/,/Detail/p' {}" > add_8_4_utilization_estimates.txt
echo "${REPORTS}" | grep axc_add | grep -- -8-4- | xargs -r -I {} sh -c "echo {}; sed -n '/Latency/,/Detail/p' {}"               > add_8_4_latency_estimates.txt
echo "${REPORTS}" | grep axc_add | grep -- -8-4- | xargs -r -I {} sh -c "echo {}; sed -n '/Timing/,/Latency/p' {}"               > add_8_4_timing_estimates.txt

echo "${REPORTS}" | grep axc_add | grep -- -16-1- | xargs -r -I {} sh -c "echo {}; sed -n '/Utilization Estimates/,/Detail/p' {}" > add_16_1_utilization_estimates.txt
echo "${REPORTS}" | grep axc_add | grep -- -16-1- | xargs -r -I {} sh -c "echo {}; sed -n '/Latency/,/Detail/p' {}"               > add_16_1_latency_estimates.txt
echo "${REPORTS}" | grep axc_add | grep -- -16-1- | xargs -r -I {} sh -c "echo {}; sed -n '/Timing/,/Latency/p' {}"               > add_16_1_timing_estimates.txt

echo "${REPORTS}" | grep axc_add | grep -- -16-2- | xargs -r -I {} sh -c "echo {}; sed -n '/Utilization Estimates/,/Detail/p' {}" > add_16_2_utilization_estimates.txt
echo "${REPORTS}" | grep axc_add | grep -- -16-2- | xargs -r -I {} sh -c "echo {}; sed -n '/Latency/,/Detail/p' {}"               > add_16_2_latency_estimates.txt
echo "${REPORTS}" | grep axc_add | grep -- -16-2- | xargs -r -I {} sh -c "echo {}; sed -n '/Timing/,/Latency/p' {}"               > add_16_2_timing_estimates.txt

echo "${REPORTS}" | grep axc_add | grep -- -16-4- | xargs -r -I {} sh -c "echo {}; sed -n '/Utilization Estimates/,/Detail/p' {}" > add_16_4_utilization_estimates.txt
echo "${REPORTS}" | grep axc_add | grep -- -16-4- | xargs -r -I {} sh -c "echo {}; sed -n '/Latency/,/Detail/p' {}"               > add_16_4_latency_estimates.txt
echo "${REPORTS}" | grep axc_add | grep -- -16-4- | xargs -r -I {} sh -c "echo {}; sed -n '/Timing/,/Latency/p' {}"               > add_16_4_timing_estimates.txt

echo "${REPORTS}" | grep axc_add | grep -- -16-8- | xargs -r -I {} sh -c "echo {}; sed -n '/Utilization Estimates/,/Detail/p' {}" > add_16_8_utilization_estimates.txt
echo "${REPORTS}" | grep axc_add | grep -- -16-8- | xargs -r -I {} sh -c "echo {}; sed -n '/Latency/,/Detail/p' {}"               > add_16_8_latency_estimates.txt
echo "${REPORTS}" | grep axc_add | grep -- -16-8- | xargs -r -I {} sh -c "echo {}; sed -n '/Timing/,/Latency/p' {}"               > add_16_8_timing_estimates.txt

# IMPORTANT: MULTIPLICATION

echo "${REPORTS}" | grep axc_mul | grep -- -8-1- | xargs -r -I {} sh -c "echo {}; sed -n '/Utilization Estimates/,/Detail/p' {}" > mul_8_1_utilization_estimates.txt
echo "${REPORTS}" | grep axc_mul | grep -- -8-1- | xargs -r -I {} sh -c "echo {}; sed -n '/Latency/,/Detail/p' {}"               > mul_8_1_latency_estimates.txt
echo "${REPORTS}" | grep axc_mul | grep -- -8-1- | xargs -r -I {} sh -c "echo {}; sed -n '/Timing/,/Latency/p' {}"               > mul_8_1_timing_estimates.txt

echo "${REPORTS}" | grep axc_mul | grep -- -8-2- | xargs -r -I {} sh -c "echo {}; sed -n '/Utilization Estimates/,/Detail/p' {}" > mul_8_2_utilization_estimates.txt
echo "${REPORTS}" | grep axc_mul | grep -- -8-2- | xargs -r -I {} sh -c "echo {}; sed -n '/Latency/,/Detail/p' {}"               > mul_8_2_latency_estimates.txt
echo "${REPORTS}" | grep axc_mul | grep -- -8-2- | xargs -r -I {} sh -c "echo {}; sed -n '/Timing/,/Latency/p' {}"               > mul_8_2_timing_estimates.txt

echo "${REPORTS}" | grep axc_mul | grep -- -8-4- | xargs -r -I {} sh -c "echo {}; sed -n '/Utilization Estimates/,/Detail/p' {}" > mul_8_4_utilization_estimates.txt
echo "${REPORTS}" | grep axc_mul | grep -- -8-4- | xargs -r -I {} sh -c "echo {}; sed -n '/Latency/,/Detail/p' {}"               > mul_8_4_latency_estimates.txt
echo "${REPORTS}" | grep axc_mul | grep -- -8-4- | xargs -r -I {} sh -c "echo {}; sed -n '/Timing/,/Latency/p' {}"               > mul_8_4_timing_estimates.txt

echo "${REPORTS}" | grep axc_mul | grep -- -16-1- | xargs -r -I {} sh -c "echo {}; sed -n '/Utilization Estimates/,/Detail/p' {}" > mul_16_1_utilization_estimates.txt
echo "${REPORTS}" | grep axc_mul | grep -- -16-1- | xargs -r -I {} sh -c "echo {}; sed -n '/Latency/,/Detail/p' {}"               > mul_16_1_latency_estimates.txt
echo "${REPORTS}" | grep axc_mul | grep -- -16-1- | xargs -r -I {} sh -c "echo {}; sed -n '/Timing/,/Latency/p' {}"               > mul_16_1_timing_estimates.txt

echo "${REPORTS}" | grep axc_mul | grep -- -16-2- | xargs -r -I {} sh -c "echo {}; sed -n '/Utilization Estimates/,/Detail/p' {}" > mul_16_2_utilization_estimates.txt
echo "${REPORTS}" | grep axc_mul | grep -- -16-2- | xargs -r -I {} sh -c "echo {}; sed -n '/Latency/,/Detail/p' {}"               > mul_16_2_latency_estimates.txt
echo "${REPORTS}" | grep axc_mul | grep -- -16-2- | xargs -r -I {} sh -c "echo {}; sed -n '/Timing/,/Latency/p' {}"               > mul_16_2_timing_estimates.txt

echo "${REPORTS}" | grep axc_mul | grep -- -16-4- | xargs -r -I {} sh -c "echo {}; sed -n '/Utilization Estimates/,/Detail/p' {}" > mul_16_4_utilization_estimates.txt
echo "${REPORTS}" | grep axc_mul | grep -- -16-4- | xargs -r -I {} sh -c "echo {}; sed -n '/Latency/,/Detail/p' {}"               > mul_16_4_latency_estimates.txt
echo "${REPORTS}" | grep axc_mul | grep -- -16-4- | xargs -r -I {} sh -c "echo {}; sed -n '/Timing/,/Latency/p' {}"               > mul_16_4_timing_estimates.txt

echo "${REPORTS}" | grep axc_mul | grep -- -16-8- | xargs -r -I {} sh -c "echo {}; sed -n '/Utilization Estimates/,/Detail/p' {}" > mul_16_8_utilization_estimates.txt
echo "${REPORTS}" | grep axc_mul | grep -- -16-8- | xargs -r -I {} sh -c "echo {}; sed -n '/Latency/,/Detail/p' {}"               > mul_16_8_latency_estimates.txt
echo "${REPORTS}" | grep axc_mul | grep -- -16-8- | xargs -r -I {} sh -c "echo {}; sed -n '/Timing/,/Latency/p' {}"               > mul_16_8_timing_estimates.txt

# IMPORTANT: file listing

echo "${REPORTS}" | grep axc_add | xargs -r -I {} sh -c "echo {}" > files_add_utilization_estimates.list
echo "${REPORTS}" | grep axc_add | xargs -r -I {} sh -c "echo {}" > files_add_latency_estimates.list
echo "${REPORTS}" | grep axc_add | xargs -r -I {} sh -c "echo {}" > files_add_timing_estimates.list

echo "${REPORTS}" | grep axc_mul | xargs -r -I {} sh -c "echo {}" > files_mul_utilization_estimates.list
echo "${REPORTS}" | grep axc_mul | xargs -r -I {} sh -c "echo {}" > files_mul_latency_estimates.list
echo "${REPORTS}" | grep axc_mul | xargs -r -I {} sh -c "echo {}" > files_mul_timing_estimates.list
