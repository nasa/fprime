# FppTest/port

This directory contains unit tests for the FPP port code generator.

* Supports unit tests for typed-to-typed, typed-to-serial, serial-to-typed, and 
serial-to-serial port connections

To use this directory, you must have installed F Prime, and you must be inside 
the F Prime Python virtual environment.

* To build the tests, run `fprime-util build --ut`.
* To run the tests, run `fprime-util check`.

## Generating Coverage

Mainline F Prime doesn't generate coverage for autocoded files in a
reliable way. This directory contains some workaround scripts for
generating coverage. To generate coverage, do the following:

```
% fprime-util build --ut
% ./build-exe
% ./exe
% ./gen-cov-all
```

The output will go to the console and to `.gcov` files. To run the
coverage analysis for one file, replace the last line with `./gen-cov`
followed by the base file name, without `.cpp` or `.hpp`. For example:

```
% ./gen-cov NoArgsPortAc
```
