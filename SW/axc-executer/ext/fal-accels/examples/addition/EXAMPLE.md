# Dataflow Addition Accelerator example

## Files

* Core: `dataflow-addition-core.cpp`
* Sources: `dataflow-addition.cpp`, `dataflow-addition.hpp`
* Testbench: `dataflow-addition-tb.cpp`

## Synthesise:

You can modify the accelerator by defining the following environment variables:

* The accelerator: `ACCELERATOR=addition`
* The length of the data type (fixed-point): `Q_BW=8`
* The length of the integer part (fixed-point): `Q_INT=1`
* The clock period: `CLOCK_PERIOD=7.5`
* The random seed: `Q_SEED=0`

* The number of approximate bits in the adder: `Q_NBA=0`
* The approximation: `Q_METHOD=0`
* Others...

For more information about these variables, please, do `make help`.

For executing the synthesis by using the modifiers mentioned above, please, take the following example as a reference:

* Addition:

```bash
make ACCELERATOR=addition Q_BW=10 Q_INT=1 Q_SEED=1 synthesis
# if you get an error in Export RTL
ACCELERATOR=addition Q_BW=10 Q_INT=1 Q_SEED=1 faketime -f  "-1y" make synthesis
```
