**Note:** auto-generated from comments in: ./autocoder/ai_ut.cmake

## autocoder/ai-ut:

Unit test support files autocoder. Only run when BUILD_TESTING is set. Otherwise it is not registered.


## `is_supported`:

Given a single input file, determines if that input file is processed by this autocoder. Sets the variable named
IS_SUPPORTED in parent scope to be TRUE if the source file is an AI XML component file or FALSE otherwise. This only
processes component ai xml files.

AC_INPUT_FILE: filepath for consideration


## get_generated_files:

This autocoder always generates TesterBase.cpp, TesterBase.hpp, and GTestBase.{c|h}pp files when INCLUDE_GTEST is set.
Sets GENERATED_FILES in parent scope to hold this information.


## get_dependencies:

No dependencies, this function is a no-op.


## setup_autocode:

Setup the autocoder build commands. This is a required function of a given autocoder implementation.


