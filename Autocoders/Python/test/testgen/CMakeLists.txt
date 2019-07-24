# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathSenderComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/MathOpPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/MathResultPortAi.xml"
)
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TesterBase.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/GTestBase.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TesterBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/GTestBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/MathSenderComponentImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/MathSenderComponentImpl.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/main.cpp"
)
register_fprime_module()
register_fprime_ut()


