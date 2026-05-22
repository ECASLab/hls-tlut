# Convolution DSE process

Variables:

* Q_BW = 8
* Q_INT = 1
* Q_K = 3
* CLOCK_PERIOD = 7.5 ns
* Q_PES = 4

DSE:

| Configuration | Optimisations                                                           | Consumptions                                            | Latency    |
|---------------|-------------------------------------------------------------------------|---------------------------------------------------------|------------|
| 0 - 8 bits    | None                                                                    | BRAM: 0 (0) FF: 6072 (5) LUT: 15379 (28) DSP: 16 (7)      | 353 cycles |
| 0 - 16 bits   | None                                                                    | BRAM: 0 (0) FF: 12259 (11) LUT: 18052 (33) DSP: 64 (29)      | 375 cycles |
| 1 - 8 bits    | * Inline and pipeline stages. Unroll data caches. Pipeline stream reads | BRAM: 0 (0) FF: 4793 (4) LUT: 10633 (19) DSP: 16 (7)    | 88 cycles  |
| 1 - 16 bits   | * Inline and pipeline stages. Unroll data caches. Pipeline stream reads | BRAM: 0 (0) FF: 10135 (9) LUT: 12786 (24) DSP: 64 (29) | 103 cycles  |
| 2 - 8 bits    | * Inline and pipeline stages.                                           | BRAM: 0 (0) FF: 4658 (4) LUT: 12380 (23) DSP: 16 (7)    | 413 cycles  |
| 2 - 16 bits   | * Inline and pipeline stages.                                           | BRAM: 0 (0) FF: 9541 (8) LUT: 14661 (27) DSP: 64 (29) | 437 cycles  |
| 3 - 8 bits    | * No-Inline and pipeline functions. Unroll data caches. Pipeline stream reads | BRAM: 0 (0) FF: 11044 (10) LUT: 14746 (27) DSP: 16 (7) | 53 cycles  |
| 3 - 16 bits   | * No-Inline and pipeline functions. Unroll data caches. Pipeline stream reads | BRAM: 0 (0) FF: 18113 (17) LUT: 17025 (32) DSP: 64 (29) | 68 cycles  |
| 4 - 8 bits    | * No-Inline and pipeline functions at top level. Unroll data caches. Pipeline stream reads | BRAM: 0 (0) FF: 10968 (10) LUT: 14697 (27) DSP: 16 (7) | 49 cycles  |
| 4 - 16 bits   | * No-Inline and pipeline functions at top level. Unroll data caches. Pipeline stream reads | BRAM: 0 (0) FF: 17968 (16) LUT: 17022 (31) DSP: 64 (29) | 64 cycles  |
| 5 - 8 bits    | * No-Inline and pipeline functions at top level. Unroll data caches. Pipeline stream reads | BRAM: 0 (0) FF: 11120 (10) LUT: 16139 (30) DSP: 16 (7) | 54 cycles  |
| 5 - 16 bits   | * No-Inline and pipeline functions at top level. Unroll data caches. Pipeline stream reads | BRAM: 0 (0) FF: 18124 (17) LUT: 18560 (34) DSP: 64 (29) | 69 cycles  |
| 6 - 8 bits    | * Dataflow - No-Inline and pipeline functions at top level. Unroll data caches. Pipeline stream reads | BRAM: 0 (0) FF: 8811 (8) LUT: 15578 (29) DSP: 16 (7) | 38 cycles  |
| 6 - 16 bits   | * Dataflow - No-Inline and pipeline functions at top level. Unroll data caches. Pipeline stream reads | BRAM: 0 (0) FF: 18100 (17) LUT: 18279 (34) DSP: 64 (29) | 47 cycles  |

## Optimisations

* 0: None
* 1: Inline + pipeline
  * Inline to stages: load data, load kernel, execute, retrieve data
  * Pipeline to all of them
* 3: Inline + pipeline + Unroll data caches + Pipeline stream reads
  * Inline to stages: load data, load kernel, execute, retrieve data
  * Pipeline to all of them
  * Unroll data caches for 2D to 1D back and forth
  * Unroll kernel load
  * Pipeline data stream reads / writes
  * Unroll package compression / decompression from streams
  * **It did not have any positive effect**
* 2: No-inline and pipeline stages + Unrolling
  * No-inline functions
  * Pipeline functions (not the upper level)
  * Unroll to data cache for kernel
  * Unroll Spatial
  * Inline execution cores
  * Unroll data caches
  * Pipeline to streams
* 4: Pipeline at top level
  * No-Inline to stages: load data, load kernel, execute, retrieve data
  * Pipeline to all of them at top level (not per function)
  * Unroll data caches for 2D to 1D back and forth
  * Pipeline data stream reads / writes
  * Unroll package compression / decompression from streams
  * Dataflow
* 5: Dataflow over all the flow
  * No-Inline to stages: load data, load kernel, execute, retrieve data
  * Pipeline each function
  * Unroll data caches for 2D to 1D back and forth
  * Pipeline data stream reads / writes
  * Unroll package compression / decompression from streams
  * Dataflow to the Core
  * No-Inline of the execute (no vectorisation)
* 6: Dataflow + static over the biggest arrays
  * Sames as 5
  * Inline of the execute
