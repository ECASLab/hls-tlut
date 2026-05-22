LUT:

Configurations:

* BW: 8 bits
* INT: 3 bits (including sign)
* Points: 64
* Domain: [-1, 1]
* Board: Zed

Taken from Execute HW Block

| Solution name                  | Clocks | LUT    | FF     | BRAM   | DSP    |
|--------------------------------|--------|--------|--------|--------|--------|
| S1 Clean directives file       | 65     | 267    | 112    | 0      | 1      |
| S2 Interfaces AXI              | 65     | 267    | 112    | 0      | 1      |
| S3 Partition interface arrays  | 58     | 1207   | 437    | 0      | 1      |
| S4 Pipelining                  | 5      | 6180   | 4614   | 0      | 32     |
| S5 PE Optimisations            | 5      | 6180   | 4614   | 0      | 32     |
| S6 Pipeline at the accel level | 7      | 1566   | 1296   | 0      | 8      |

Taken from top

| Solution name                  | Clocks | LUT    | FF     | BRAM   | DSP    | Observations                               |
|--------------------------------|--------|--------|--------|--------|--------|--------------------------------------------|
| S1 Clean directives file       | 600    | 2879   | 612    | 0      | 2      | Non parallelism                            |
| S2 Interfaces AXI              | 604    | 3962   | 1825   | 0      | 2      | Non parallelism                            |
| S3 Partition interface arrays  | 453    | 8888   | 5544   | 0      | 4      | The units are splitted without parallelism |
| S4 Pipeline                    | 16     | 8448   | 7450   | 0      | 33     | The number of clocks is 16 / 24 ops |
| S5 PE Optimisations            | 17     | 8468   | 7773   | 0      | 33     | The number of clocks is 17 / 24 ops |
| S6 Pipeline at the accel level | 19     | 4071   | 4317   | 0      | 9      | Much less resources |

Command:

```
SEED=6
ACCELERATOR=activations Q_BW=8 Q_INT=3 Q_SEED=${SEED} Q_CORE=TanH faketime -f  "-1y" make synthesis
```
