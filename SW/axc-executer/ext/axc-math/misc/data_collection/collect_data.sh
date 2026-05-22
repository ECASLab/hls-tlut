#!/usr/bin/env sh

# this script is used to collect data from Vivado reports, it goes together
# with `create_datasets.sh`
#
# this script requires OPS_PATH to be defined, and be where the
# approximate-math-operators repository is contained

# IMPORTANT WARNING:
# this script runs git clean, so DO NOT RUN IT if you have unstaged changes
# that are important

if [ -z ${OPS_PATH} ]; then
	echo "OPS_PATH is not defined"
	exit 1
fi

set -xe

cd ${OPS_PATH}/approximate-math-operators/tests/hw-ops/tests || exit 1

git clean -dfx

tests="axc_add axc_multiply"
sizes="8 16" # NOTE: 8 and 8 would be meaningless in the data, so separate that into another block
bits="1 2 4"
methods="EXACT DROP OR"
samples="1000"

for t in $tests; do
  export t=$t
  for z in $sizes; do
    export z=$z
    for b in $bits; do
      export b=$b
      for m in $methods; do
        export m=$m
        for s in $samples; do
          export s=$s
          make clean-all
          Q_BW="$z" Q_INT=2 Q_O="$s" TEST="$t" AXC_BITS="$b" METHOD="$m" make
          dest="/tmp/data/$(date +%F_%H-%M-%S)-$t-$z-$b-$m-$s"
          echo "dest: $dest"
          mkdir -p "$dest"
		  # added to avoid problem where vivado_hls doesn't finish in time, and a log is lost
          sleep 1
          find . -name hls_project\* | xargs -r -I {} rsync -r {} "$dest"
          unset s
        done
        unset m
      done
      unset b
    done
    unset z
  done
  unset t
done

tests="axc_add axc_multiply"
sizes="16" # NOTE: 8 and 8 will be meaningless in the data
bits="8"
methods="EXACT DROP OR"
samples="1000"

for t in $tests; do
  export t=$t
  for z in $sizes; do
    export z=$z
    for b in $bits; do
      export b=$b
      for m in $methods; do
        export m=$m
        for s in $samples; do
          export s=$s
          make clean-all
          Q_BW="$z" Q_INT=2 Q_O="$s" TEST="$t" AXC_BITS="$b" METHOD="$m" make
          dest="/tmp/data/$(date +%F_%H-%M-%S)-$t-$z-$b-$m-$s"
          echo "dest: $dest"
          mkdir -p "$dest"
		  # added to avoid problem where vivado_hls doesn't finish in time, and a log is lost
          sleep 1
          find . -name hls_project\* | xargs -r -I {} rsync -r {} "$dest"
          unset s
        done
        unset m
      done
      unset b
    done
    unset z
  done
  unset t
done

datadir="${OPS_PATH}/data$(date +%F_%H-%M-%S)"
mkdir -p "${datadir}"
mv /tmp/data "${datadir}"
