# CMake generated Testfile for 
# Source directory: /Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/Fw/Types
# Build directory: /Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/F-Prime/Fw/Types
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(Fw_Types_ut_exe "/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/bin/Darwin/Fw_Types_ut_exe")
set_tests_properties(Fw_Types_ut_exe PROPERTIES  _BACKTRACE_TRIPLES "/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/cmake/support/Unit_Test.cmake;132;add_test;/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/cmake/API.cmake;443;generate_ut;/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/Fw/Types/CMakeLists.txt;33;register_fprime_ut;/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/Fw/Types/CMakeLists.txt;0;")
add_test(Fw_Types_StringUtils "/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/bin/Darwin/Fw_Types_StringUtils")
set_tests_properties(Fw_Types_StringUtils PROPERTIES  _BACKTRACE_TRIPLES "/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/cmake/support/Unit_Test.cmake;132;add_test;/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/cmake/API.cmake;443;generate_ut;/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/Fw/Types/CMakeLists.txt;38;register_fprime_ut;/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/Fw/Types/CMakeLists.txt;0;")
subdirs("GTest")
