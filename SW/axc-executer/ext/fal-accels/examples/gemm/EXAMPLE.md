# GEMM example

## Files

* Core: `stream-gemm-core.cpp`
* Sources: `stream-gemm.cpp`, `stream-gemm.hpp`
* Testbench: `stream-gemm-tb.cpp`

## Synthesise:

You can modify the accelerator by defining the following environment variables:

* The accelerator: `ACCELERATOR=gemm`
* The length of the data type (fixed-point): `Q_BW=8`
* The length of the integer part (fixed-point): `Q_INT=1`
* The output size: `Q_O=2`
* The number of PEs within the accelerator: `Q_PES=4`
* The clock period: `CLOCK_PERIOD=7.5`
* The random seed: `Q_SEED=0`
* Others...

For more information about these variables, please, do `make help`.

For executing the synthesis by using the modifiers mentioned above, please, take the following example as a reference:

* GEMM:

```bash
make ACCELERATOR=gemm Q_BW=10 Q_INT=1 Q_SEED=1 synthesis
# if you get an error in Export RTL
ACCELERATOR=gemm Q_BW=10 Q_INT=1 Q_SEED=1 faketime -f  "-1y" make synthesis
```
