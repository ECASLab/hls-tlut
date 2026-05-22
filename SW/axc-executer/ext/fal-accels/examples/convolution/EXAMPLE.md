# Convolution example

## Files

* Core: `stream-convolution-core.cpp`
* Sources: `stream-convolution.cpp`, `stream-convolution.hpp`
* Testbench: `stream-convolution-tb.cpp`

## Synthesise:

You can modify the accelerator by defining the following environment variables:

* The accelerator: `ACCELERATOR=convolution`
* The kernel size (convolution only): `Q_KS=3`
* The length of the data type (fixed-point): `Q_BW=8`
* The length of the integer part (fixed-point): `Q_INT=1`
* The output size: `Q_O=2`
* The PE core: `Q_CORE=Spatial`
* The number of PEs within the accelerator: `Q_PES=4`
* The clock period: `CLOCK_PERIOD=7.5`
* The random seed: `Q_SEED=0`
* Others...

For more information about these variables, please, do `make help`.

For executing the synthesis by using the modifiers mentioned above, please, take the following example as a reference:

* Convolution:

```bash
make ACCELERATOR=convolution Q_BW=10 Q_INT=2 Q_CORE=Spatial synthesis
# if you get an error in Export RTL
ACCELERATOR=convolution Q_BW=10 Q_INT=2 Q_CORE=Spatial faketime -f  "-1y" make synthesis
```
