**Note:** auto-generated from comments in: ./autocoder/fpp_ut.cmake

## autocoder/fpp.cmake:

CMake implementation of an fprime autocoder. Includes the necessary function definitions to implement the fprime
autocoder API and wraps calls to the FPP tools.


## Function `fpp_ut_is_supported`:

Runs on any .fpp file.


## Function `fpp_ut_setup_autocode`:

Sets up the steps to run the autocoder and produce the files during the build. This is passed the lists generated
in calls to `get_generated_files` and `get_dependencies`.

AC_INPUT_FILES: list of supported autocoder input files


