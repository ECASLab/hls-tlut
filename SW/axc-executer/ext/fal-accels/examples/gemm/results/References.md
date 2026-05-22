# Convolution DSE process

Variables:

* Q_BW = 8, 16
* Q_INT = 2
* Q_O = 2
* CLOCK_PERIOD = 7.5 ns
* Q_PES = 4

DSE:

| Configuration | Optimisations                                                           | Consumptions                                            | Latency    |
|---------------|-------------------------------------------------------------------------|---------------------------------------------------------|------------|
| 0 - 8 bits    | None                                                                    | BRAM: 0 (0) FF: 1561 (1) LUT: 4091 (7) DSP: 0 (0)       | 598 cycles |
| 0 - 16 bits   | None                                                                    | BRAM: 0 (0) FF: 1727 (1) LUT: 4071 (7) DSP: 1 (0)       | 646 cycles |
| 1 - 8 bits    | Pipeline execution, unroll inner execute loops                          | BRAM: 3 (1) FF: 1811 (1) LUT: 5028 (9) DSP: 0 (0)       | 407 cycles |
| 1 - 16 bits   | Pipeline execution, unroll inner execute loops                          | BRAM: 1 (0) FF: 2397 (1) LUT: 4797 (9) DSP: 8 (3)       | 427 cycles |
| 2 - 8 bits    | Pipeline execution, unroll inner execute loops                          | BRAM: 0 (0) FF: 5758 (5) LUT: 10903 (20) DSP: 0 (0)     | 395 cycles |
| 2 - 16 bits   | Pipeline execution, unroll inner execute loops                          | BRAM: 0 (0) FF: 9114 (8) LUT: 11017 (20) DSP: 8 (3)     | 411 cycles |
| 3 - 8 bits    | Pipeline per function, unroll inner execute loops                       | BRAM: 0 (0) FF: 5456 (5) LUT: 8218 (15) DSP: 0 (0)      | 29 cycles  |
| 3 - 16 bits   | Pipeline per function, unroll inner execute loops                       | BRAM: 0 (0) FF: 9341 (8) LUT: 8960 (16) DSP: 8 (3)      | 37 cycles  |
| 4 - 8 bits    | Pipeline per function, unroll inner execute loops, vectorisation        | BRAM: 0 (0) FF: 3799 (3) LUT: 5067 (9) DSP: 0 (0)       | 12 cycles  |
| 4 - 16 bits   | Pipeline per function, unroll inner execute loops, vectorisation        | BRAM: 0 (0) FF: 7604 (7) LUT: 4839 (9) DSP: 32 (14)     | 20 cycles  |
| 5 - 8 bits    | Pipeline per core scope, unroll inner execute loops, vectorisation      | BRAM: 0 (0) FF: 3405 (3) LUT: 5105 (9) DSP: 0 (0)       | 10 cycles  |
| 5 - 16 bits   | Pipeline per core scope, unroll inner execute loops, vectorisation      | BRAM: 0 (0) FF: 6826 (6) LUT: 4877 (9) DSP: 32 (14)       | 18 cycles  |


## Optimisations

* 0: None
* 1: Optimise execute:
  * Pipeline Execute function
  * Unroll inner loops at the PE level
* 2: Partition arrays
  * Optimisation 1
  * Partition interfaces: outter arrays completely
  * Partition cache buffers
* 3: Data retrieval optimisation
  * Optimisation 2
  * Pipelining to data retrieval functions
  * Inlining to receive matrix
* 4: Vectorise
  * Optimisation 3
  * Vectorisation (inline)
* 5: Pipeline at core level without pipelines per function
  * Optimisation 4
  * Pipeline at core level
