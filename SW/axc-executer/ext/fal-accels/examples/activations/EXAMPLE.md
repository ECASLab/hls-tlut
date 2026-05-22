# Activations example

## Files

* Core: `stream-activations-core.cpp`
* Sources: `stream-activations.cpp`, `stream-activations.hpp`
* Testbench: `stream-activations-tb.cpp`

## Synthesise:

You can modify the accelerator by defining the following environment variables:

* The accelerator: `ACCELERATOR=activations`
* The length of the data type (fixed-point): `Q_BW=8`
* The length of the integer part (fixed-point): `Q_INT=1`
* The output size: `Q_O=2`. In this context, it will define the number of COLS only
* The number of PEs within the accelerator: `Q_PES=4`
* The clock period: `CLOCK_PERIOD=7.5`
* The random seed: `Q_SEED=0`
* The activation core: `Q_CORE={TanH}` (currently unused)
* Others...

> The activations are based with MatrixMap (except for Softmax)

For more information about these variables, please, do `make help`.

For executing the synthesis by using the modifiers mentioned above, please, take the following example as a reference:

* activations:

```bash
make ACCELERATOR=activations Q_BW=10 Q_INT=1 Q_SEED=1 Q_CORE=TanH synthesis
#if you get an error in Export RTL
ACCELERATOR=activations Q_BW=10 Q_INT=1 Q_SEED=1 Q_CORE=TanH faketime -f  "-1y" make synthesis
```
