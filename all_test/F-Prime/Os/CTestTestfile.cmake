# CMake generated Testfile for 
# Source directory: /Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/Os
# Build directory: /Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/F-Prime/Os
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(Os_pthreads "/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/bin/Darwin/Os_pthreads")
set_tests_properties(Os_pthreads PROPERTIES  _BACKTRACE_TRIPLES "/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/cmake/support/Unit_Test.cmake;132;add_test;/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/cmake/API.cmake;443;generate_ut;/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/Os/CMakeLists.txt;107;register_fprime_ut;/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/Os/CMakeLists.txt;0;")
add_test(Os_pthreads_max_heap "/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/bin/Darwin/Os_pthreads_max_heap")
set_tests_properties(Os_pthreads_max_heap PROPERTIES  _BACKTRACE_TRIPLES "/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/cmake/support/Unit_Test.cmake;132;add_test;/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/cmake/API.cmake;443;generate_ut;/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/Os/CMakeLists.txt;113;register_fprime_ut;/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/Os/CMakeLists.txt;0;")
subdirs("Stubs")
