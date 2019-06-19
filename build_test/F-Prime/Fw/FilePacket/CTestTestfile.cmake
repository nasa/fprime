# CMake generated Testfile for 
# Source directory: /Users/jishii/Documents/rep_other/fprime-sw/Fw/FilePacket
# Build directory: /Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Fw/FilePacket
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(Fw_FilePacket_ut_exe "/Users/jishii/Documents/rep_other/fprime-sw/build_test/bin/Darwin/Fw_FilePacket_ut_exe")
set_tests_properties(Fw_FilePacket_ut_exe PROPERTIES  _BACKTRACE_TRIPLES "/Users/jishii/Documents/rep_other/fprime-sw/cmake/support/Unit_Test.cmake;114;add_test;/Users/jishii/Documents/rep_other/fprime-sw/cmake/API.cmake;413;generate_ut;/Users/jishii/Documents/rep_other/fprime-sw/Fw/FilePacket/CMakeLists.txt;39;register_fprime_ut;/Users/jishii/Documents/rep_other/fprime-sw/Fw/FilePacket/CMakeLists.txt;0;")
subdirs("GTest")
