# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathSenderComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/MathOpPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/MathResultPortAi.xml"
)
register_fprime_module()

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathSenderComponentImpl.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/MathSenderComponentImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TesterBase.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TesterBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/GTestBase.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/GTestBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
)
set(EXECUTABLE_NAME "Autocoders_Python_test_testgen_ut_exe")
set(MOD_DEPS
  "${CMAKE_CURRENT_LIST_DIR}"
  gtest
)
register_fprime_executable()