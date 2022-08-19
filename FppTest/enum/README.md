# FppTest/enum

This directory contains unit tests for the FPP enum code generator.

* EnumToStringTest: Tests enum toString() and ostream operator functions
* IsValidTest: Additional tests for isValid() function with boundary values

To use this directory, you must have installed F Prime, and you must be inside 
the F Prime Python virtual environment.

* To build the tests, run `fprime-util build --ut`.
* To run the tests, run `fprime-util check`.
