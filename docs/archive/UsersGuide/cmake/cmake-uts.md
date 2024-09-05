# CMake Build System Unit Tests

These tests seek to ensure that the CMake system functions as expected. These will test that the
CMake system meets all the requirements specified in the CMake SDD. The goal is to ensure
that the CMake system can be maintainable by ensuring core functions of the CMake process work as
expected in an automated system.

## Implementation Via PyTest

The CMake system runs its tests via PyTest. This allows the implementation to be streamlined using
standard a testing framework. Python `subprocess` is used to run CMake for its simplicity. To prepare to run these tests, ensure that you are running fprime out of a virtual environment following the fprime installation. Then run `pytest` in this directory: `cd cmake/test; pytest`

**Running:**
```
fprime> cd cmake/test
pytest
```