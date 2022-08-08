# FppTest/test/array

This directory contains unit tests for the FPP array code generator.

* StringArrayTest: Tests an array of string values
* ArrayArrayTest: Tests an array of arrays
* EnumArrayTest: Tests an array of enum values
* StructArrayTest: Tests an array of struct values
* StringTest: Tests a nested string class within a generated array class
* FormatTest: Tests FPP format strings

To use this directory, you must have installed F Prime, and you must be inside 
the F Prime Python virtual environment.

* To build the tests, run 'fprime-util build --ut'.
* To run the tests, run 'fprime-util check'.
