# generate_luts.py

This script generates LUT tables for all possible Q format combinations ($Q I.F$) based on target bit precision. It explores formats to find the one with the lowest **Mean Squared Error (MSE)** for a given function.

## How it works

- **Domain Optimization:** You can optimize the LUT for symmetric (e.g., `tanh`) or positive-only (e.g., `exp`) functions.
- **Precision Gain:** When optimized, the script treats values as unsigned. The bit previously used for the sign is repurposed as an extra fractional bit ($F+1$), effectively **halving the step size** and doubling resolution.

## Q Format Formulas

Assuming a standard format **Q$I.F$** where total bits $N = I + F$ ($I$ includes the sign bit):

- **Step (Resolution):**
  - Standard (Signed): $\text{step} = 2^{-F}$
  - Optimized (Unsigned): $\text{step} = 2^{-(F+1)}$
- **Representable Range:**
  - Standard (Two's complement): $[-2^{I-1}, 2^{I-1} - \text{step}]$
  - Optimized (Positive only): $[0, 2^{I-1} - \text{step}]$



## Example: 4 bits, Q1.3 for tanh

### Without optimization (Signed Q1.3)
- **Format:** 1 sign bit + 3 fractional bits.
- **Step:** $2^{-3} = 0.125$
- **Range:** $[-1.000, 0.875]$

| Index | Value   |
|-------|---------|
| 0     | -1.000  |
| 1     | -0.875  |
| ...   | ...     |
| 8     |  0.000  |
| 15    |  0.875  |

### With optimization (Unsigned UQ0.4)
By repurposing the sign bit, we gain precision. The new maximum is the previous maximum plus the new halved step ($0.875 + 0.0625$).

- **New Step:** $0.125 / 2 = 0.0625$ ($2^{-4}$)
- **New Range:** $[0.0000, 0.9375]$

| Index | Value   |
|-------|---------|
| 0     | 0.0000  |
| 1     | 0.0625  |
| 2     | 0.1250  |
| ...   | ...     |
| 14    | 0.8750  |
| 15    | 0.9375  |