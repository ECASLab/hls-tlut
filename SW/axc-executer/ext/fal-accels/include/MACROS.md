# Accelerator Macros

The accelerators library depends on the following repos:

* [Convolution](https://gitlab.com/ecas-lab-tec/approximate-flexible-acceleration-ml/approximate-convolution-accelerator)
* [Math](https://gitlab.com/ecas-lab-tec/approximate-flexible-acceleration-ml/approximate-math-operators.git)
* [Matrix](https://gitlab.com/ecas-lab-tec/approximate-flexible-acceleration-ml/approximate-gemm-accelerator.git)

Apart from that, this repository provides macros required to describe an accelerator, standardising the description in terms of:

* Communication
* Description of parameters and capabilities
* The core description

Within the framework, it is possible to define testbenches, hardware files and core files, along with the optimisations in the form of directive files (.tcl).

You can have a look at the [examples](./examples.html) folder for the use cases.

## Macros

The macros can be categorised in:

* Enumerators (enums.hpp): defines masks, indices, operators, execution modes, data types, and more.
* Types (type.hpp): defines data types and constants
* General Macros (macros.hpp): define preprocessor macros for accessing and writing capabilities, parameters, and checks.
* Accelerator Specific: define wrappers required to define accelerators such as convolutions and matrices. You can find more details in:
  * Convolution (convolution.hpp): for convolution accelerators
  * Matrix (matrix.hpp): for matrix-based accelerators (i.e. GEMMA)
