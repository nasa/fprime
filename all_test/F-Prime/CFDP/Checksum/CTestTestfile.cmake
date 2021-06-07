# CMake generated Testfile for 
# Source directory: /Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/CFDP/Checksum
# Build directory: /Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/F-Prime/CFDP/Checksum
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(CFDP_Checksum_ut_exe "/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/bin/Darwin/CFDP_Checksum_ut_exe")
set_tests_properties(CFDP_Checksum_ut_exe PROPERTIES  _BACKTRACE_TRIPLES "/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/cmake/support/Unit_Test.cmake;132;add_test;/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/cmake/API.cmake;443;generate_ut;/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/CFDP/Checksum/CMakeLists.txt;26;register_fprime_ut;/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/CFDP/Checksum/CMakeLists.txt;0;")
subdirs("GTest")
