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
| 0 - 8 bits    | None                                                                    | BRAM: 0 (0) FF: 4610 (4) LUT: 6143 (11) DSP: 0 (0)      | 764 cycles |
| 0 - 16 bits   | None                                                                    | BRAM: 0 (0) FF: 7587 (7) LUT: 6613 (12) DSP: 1 (0)      | 922 cycles |
| 1 - 8 bits    | * Inline and pipeline stages                                            | BRAM: 0 (0) FF: 10619 (9) LUT: 17267 (32) DSP: 1 (0)    | 34 cycles  |
| 1 - 16 bits   | * Inline and pipeline stages                                            | BRAM: 0 (0) FF: 14344 (13) LUT: 16273 (30) DSP: 36 (16) | 41 cycles  |
| 2 - 8 bits    | * Inline and pipeline stages. Unroll data caches. Pipeline stream reads | BRAM: 0 (0) FF: 10619 (9) LUT: 17267 (32) DSP: 1 (0)    | 34 cycles  |
| 2 - 16 bits   | * Inline and pipeline stages. Unroll data caches. Pipeline stream reads | BRAM: 0 (0) FF: 14344 (13) LUT: 16273 (30) DSP: 36 (16) | 41 cycles  |
| 3 - 8 bits    | * No-Inline and pipeline functions. Unroll data caches. Pipeline stream reads | BRAM: 0 (0) FF: 6073 (5) LUT: 10343 (19) DSP: 0 (0) | 16 cycles  |
| 3 - 16 bits   | * No-Inline and pipeline functions. Unroll data caches. Pipeline stream reads | BRAM: 0 (0) FF: 10510 (9) LUT: 4589 (8) DSP: 144 (65) | 23 cycles  |
| 4 - 8 bits    | * Inline and pipeline functions. Unroll data caches. Pipeline stream reads | BRAM: 0 (0) FF: 3860 (3) LUT: 9387 (17) DSP: 0 (0) | 18 cycles  |
| 4 - 16 bits   | * Inline and pipeline functions. Unroll data caches. Pipeline stream reads | BRAM: 0 (0) FF: 9688 (9) LUT: 4591 (8) DSP: 144 (65) | 25 cycles  |
| 5 - 8 bits    | * Dataflow, no-inline and pipeline functions. Unroll data caches. Pipeline stream reads | BRAM: 0 (0) FF: 6196 (6) LUT: 11554 (22) DSP: 0 (0) | 18 cycles  |
| 6 - 8 bits    | * Scoped Dataflow, no-inline and pipeline functions. Unroll data caches. Pipeline stream reads | BRAM: 0 (0) FF: 6196 (6) LUT: 11554 (22) DSP: 0 (0) | 18 cycles  |
| 6 - 16 bits   | * Scoped Dataflow, no-inline and pipeline functions. Unroll data caches. Pipeline stream reads | BRAM: 0 (0) FF: 9689 (9) LUT: 5591 (10) DSP: 144 (65) | 24 cycles  |
| 7 - 8 bits    | * Scoped Dataflow, no-inline and pipeline functions. Unroll data caches. Pipeline stream reads. No vectorisation | BRAM: 0 (0) FF: 7363 (6) LUT: 15297 (28) DSP: 0 (0) | 38 cycles  |
| 7 - 16 bits   | * Scoped Dataflow, no-inline and pipeline functions. Unroll data caches. Pipeline stream reads. No vectorisation | BRAM: 0 (0) FF: 10891 (10) LUT: 14373 (27) DSP: 36 (16) | 45 cycles  |

## Optimisations

* 0: None
* 1: Inline + pipeline
  * Inline to stages: load data, load kernel, execute, retrieve data
  * Pipeline to all of them
* 2: Inline + pipeline + Unroll data caches + Pipeline stream reads
  * Inline to stages: load data, load kernel, execute, retrieve data
  * Pipeline to all of them
  * Unroll data caches for 2D to 1D back and forth
  * Unroll kernel load
  * Pipeline data stream reads / writes
  * Unroll package compression / decompression from streams
  * **It did not have any positive effect**
* 3: No-inline and pipeline stages + Unrolling
  * No-inline functions
  * Pipeline functions (not the upper level)
  * Unroll to data cache for kernel
  * Unroll Spatial
  * Inline execution cores
  * Unroll data caches
  * Pipeline to streams
* 4: Dataflow over all the accelerator
  * Sames as 3
  * Dataflow to the Core
* 5: Dataflow over all the flow
  * Sames as 3
  * Dataflow to the Core with scoped over the processing flow 
* 6: Dataflow + static over the biggest arrays
  * Sames as 5
  * Dataflow to the Core with scoped over the processing flow 
  * Static to the outter arrays
* 7: No vectorisation
  * Removes the vectorisation
  * Same as 6
