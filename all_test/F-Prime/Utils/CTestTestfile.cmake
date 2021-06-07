# CMake generated Testfile for 
# Source directory: /Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/Utils
# Build directory: /Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/F-Prime/Utils
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(Utils_ut_exe "/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/bin/Darwin/Utils_ut_exe")
set_tests_properties(Utils_ut_exe PROPERTIES  _BACKTRACE_TRIPLES "/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/cmake/support/Unit_Test.cmake;132;add_test;/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/cmake/API.cmake;443;generate_ut;/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/Utils/CMakeLists.txt;28;register_fprime_ut;/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/Utils/CMakeLists.txt;0;")
subdirs("Hash")
subdirs("Types")
