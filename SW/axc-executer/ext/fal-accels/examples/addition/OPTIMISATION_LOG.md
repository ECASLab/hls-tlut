LUT:

Configurations:

* BW: 14 bits
* INT: 6 bits (including sign)
* Domain: [-8, 9]
* Board: Zed

Taken from Execute HW Block

| Solution name                   | Clocks | LUT    | FF     | BRAM   | DSP    |
|---------------------------------|--------|--------|--------|--------|--------|
| S1 Clean directives file        | 5      | 162    | 65     | 0      | 0      |
| S2 Partition interface arrays   | 1      | 76     | 0      | 0      | 0      |
| S3 Pipeline optimisation on top | 1      | 76     | 0      | 0      | 0      |
| S4 Pipeline on streaming        | 1      | 76     | 0      | 0      | 0      |


Taken from top

| Solution name                   | Clocks | LUT    | FF     | BRAM   | DSP    | Observations                               |
|---------------------------------|--------|--------|--------|--------|--------|--------------------------------------------|
| S1 Clean directives file        | 29     | 3368   | 1842   | 0      | 0      | Non parallelism / per iteration            |
| S2 Partition interface arrays   | 29     | 3368   | 1842   | 0      | 0      | No changes overall                         |
| S3 Pipeline optimisation on top | 3/34   | 1598   | 1286   | 0      | 0      | This pipelines on top of the function      |
| S4 Pipeline on streaming        | 3/16   | 1617   | 1277   | 0      | 0      | This pipelines on the stream loop          |


1. PE optimisations here are already covered by the pipelining, since the loops unrolled automatically
2. No intermediate buffers needed

Command:

```
ACCELERATOR=addition Q_INT=5 Q_BW=9 Q_MIN_NUM=-8 Q_MAX_NUM=9 Q_NBA=2 Q_AMETHOD=1 DSE=1 make synthesis
```

Approximation effects:

| Approximation type | NBA    | NBM    | LUT    | FF     | BRAM   | DSP    | Mean Error | Std      | Res. Gain | Ov.all Gain |
|--------------------|--------|--------|--------|--------|--------|--------|------------|----------|-----------|-------------|
| Exact (None)       | 0      | N/A    | 76     | 0      | 0      | 0      | 0.003914   | 0.001590 |  0.00 %   | 0.00 %      |
| LSBDrop            | 1      | N/A    | 68     | 0      | 0      | 0      | 0.007829   | 0.003190 | 10.53 %   | 0.49 %      |
| LSBDrop            | 2      | N/A    | 48     | 0      | 0      | 0      | 0.015651   | 0.006375 | 36.84 %   | 1.73 %      |
| LSBDrop            | 3      | N/A    | 52     | 0      | 0      | 0      | 0.031048   | 0.012776 |  -.-  %   |             |
| LSBDrop            | 4      | N/A    | 56     | 0      | 0      | 0      | 0.062204   | 0.025469 |  -.-  %   |             |
| LSBDrop            | 6      | N/A    | 60     | 0      | 0      | 0      | 0.249669   | 0.100986 |  -.-  %   |             |
| LSBOr              | 1      | N/A    | 76     | 0      | 0      | 0      | 0.004894   | 0.002331 |  -.-  %   |             |
| LSBOr              | 2      | N/A    | 56     | 0      | 0      | 0      | 0.006839   | 0.004119 |  -.-  %   |             |
| LSBOr              | 4      | N/A    | 72     | 0      | 0      | 0      | 0.018358   | 0.015600 |  -.-  %   |             |
| LSBOr              | 6      | N/A    | 84     | 0      | 0      | 0      | 0.064701   | 0.062204 |  -.-  %   |             |


> The resources are measured using the execution stage.

Questions:

* Does bigger accelerators yield to better overall gains since the execution part is bigger?
