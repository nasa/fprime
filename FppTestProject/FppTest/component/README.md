# FppTest/component

This directory contains unit tests for the FPP component code generator.

To use this directory, you must have installed F Prime, and you must be inside 
the F Prime Python virtual environment.

The following subdirectories contain tests that you can run:

* `active`: Active component tests
* `passive`: Passive component tests
* `queued`: Queued component tests
* `empty`: Basic tests of an empty component

In any of these directories, you can do the following:

* To build the tests, run `fprime-util build --ut`.
* To run the tests, run `fprime-util check`.
