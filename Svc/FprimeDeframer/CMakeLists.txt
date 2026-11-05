####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####


register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/FprimeDeframer.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/FprimeDeframer.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/FprimeDeframer.hpp"
  DEPENDS
    Utils_Types
    Svc_FprimeProtocol
)


#### UTs ####

register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/FprimeDeframer.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FprimeDeframerTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FprimeDeframerTestMain.cpp"
  DEPENDS
    STest
  UT_AUTO_HELPERS
)
