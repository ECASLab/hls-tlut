#!/usr/bin/env bash
set -euo pipefail

DEVICE_BDF="${1:-0000:c4:00.1}"
# AQUÍ ESTÁ EL CAMBIO: Se colocó tu comando de LeNet-5 con sus argumentos
APP_CMD="${2:-cd axc-executer && ./builddir/examples/benchmark-lenet5/benchmark-lenet5 examples/lenet5/model-weights examples/lenet5/model-tests/mnist-input-10000.bin 1000 examples/lenet5/model-tests/mnist-output-10000.bin 3}"
IDLE_SAMPLES="${3:-10}"
ACTIVE_SAMPLES="${4:-30}"
SLEEP_SEC="${5:-0.2}"

extract_power() {
  xrt-smi examine -d "$DEVICE_BDF" -r electrical 2>/dev/null \
    | awk -F: '/^[[:space:]]*Power[[:space:]]*:/ {
        gsub(/Watts/,"",$2);
        gsub(/^[ \t]+|[ \t]+$/,"",$2);
        print $2;
        exit
      }'
}

avg_file() {
  awk '{s+=$1; n++} END {if(n>0) printf "%.6f\n", s/n; else print "0"}' "$1"
}

max_file() {
  awk 'BEGIN{m=0} {if($1>m)m=$1} END {printf "%.6f\n", m}' "$1"
}

IDLE_LOG="idle_power.log"
ACTIVE_LOG="active_power.log"

rm -f "$IDLE_LOG" "$ACTIVE_LOG"

echo "Measuring idle power..."
for ((i=0; i<IDLE_SAMPLES; i++)); do
  extract_power >> "$IDLE_LOG"
  sleep "$SLEEP_SEC"
done

IDLE_AVG=$(avg_file "$IDLE_LOG")
IDLE_MAX=$(max_file "$IDLE_LOG")

echo "Idle average power: $IDLE_AVG W"
echo "Idle max power:     $IDLE_MAX W"

echo
echo "Launching app: $APP_CMD"
bash -c "$APP_CMD" > app_output.log 2>&1 &
APP_PID=$!

echo "Sampling active power while app runs..."
COUNT=0
while kill -0 "$APP_PID" 2>/dev/null; do
  extract_power >> "$ACTIVE_LOG" || true
  sleep "$SLEEP_SEC"
  COUNT=$((COUNT+1))
done

wait "$APP_PID" || true

# Si el programa terminó demasiado rápido y no alcanzó muchas muestras,
# completar hasta ACTIVE_SAMPLES no tiene sentido, así que usamos lo que haya.
if [[ ! -s "$ACTIVE_LOG" ]]; then
  echo "No active power samples captured."
  exit 1
fi

ACTIVE_AVG=$(avg_file "$ACTIVE_LOG")
ACTIVE_MAX=$(max_file "$ACTIVE_LOG")
DELTA_AVG=$(awk -v a="$ACTIVE_AVG" -v b="$IDLE_AVG" 'BEGIN{printf "%.6f\n", a-b}')
DELTA_MAX=$(awk -v a="$ACTIVE_MAX" -v b="$IDLE_AVG" 'BEGIN{printf "%.6f\n", a-b}')

echo
echo "===== POWER SUMMARY ====="
echo "Idle avg power:      $IDLE_AVG W"
echo "Active avg power:    $ACTIVE_AVG W"
echo "Active peak power:   $ACTIVE_MAX W"
echo "Delta avg power:     $DELTA_AVG W"
echo "Delta peak power:    $DELTA_MAX W"
echo "Active samples:      $(wc -l < "$ACTIVE_LOG")"
echo
echo "Logs:"
echo "  idle power -> $IDLE_LOG"
echo "  active power -> $ACTIVE_LOG"
echo "  app output -> app_output.log"