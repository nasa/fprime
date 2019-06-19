# CMake generated Testfile for 
# Source directory: /Users/jishii/Documents/rep_other/fprime-sw/Os
# Build directory: /Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Os
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(Os_pthreads "/Users/jishii/Documents/rep_other/fprime-sw/build_test/bin/Darwin/Os_pthreads")
set_tests_properties(Os_pthreads PROPERTIES  _BACKTRACE_TRIPLES "/Users/jishii/Documents/rep_other/fprime-sw/cmake/support/Unit_Test.cmake;114;add_test;/Users/jishii/Documents/rep_other/fprime-sw/cmake/API.cmake;413;generate_ut;/Users/jishii/Documents/rep_other/fprime-sw/Os/CMakeLists.txt;185;register_fprime_ut;/Users/jishii/Documents/rep_other/fprime-sw/Os/CMakeLists.txt;0;")
add_test(Os_pthreads_max_heap "/Users/jishii/Documents/rep_other/fprime-sw/build_test/bin/Darwin/Os_pthreads_max_heap")
set_tests_properties(Os_pthreads_max_heap PROPERTIES  _BACKTRACE_TRIPLES "/Users/jishii/Documents/rep_other/fprime-sw/cmake/support/Unit_Test.cmake;114;add_test;/Users/jishii/Documents/rep_other/fprime-sw/cmake/API.cmake;413;generate_ut;/Users/jishii/Documents/rep_other/fprime-sw/Os/CMakeLists.txt;191;register_fprime_ut;/Users/jishii/Documents/rep_other/fprime-sw/Os/CMakeLists.txt;0;")
subdirs("Stubs")
