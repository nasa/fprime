# FppTest/array

This directory contains unit tests for the FPP array code generator.

* FormatTest: Tests FPP format strings

To use this directory, you must have installed F Prime, and you must be inside 
the F Prime Python virtual environment.

* To build the tests, run `fprime-util build --ut`.
* To run the tests, run `fprime-util check`.

To use the ArrayTest suite, instantiate it with a list of types.

```c++
#include "FppTest/array/ArrayTest.hpp"
```
