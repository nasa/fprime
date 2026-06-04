####
# FPrime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
# UT_SOURCE_FILES: list of source files for unit tests
#
# More information in the F´ CMake API documentation:
# https://fprime.jpl.nasa.gov/latest/documentation/reference
#
####

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/FprimeFramer.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/FprimeFramer.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/FprimeFramer.hpp"
  DEPENDS
    Svc_FprimeProtocol
)


### Unit Tests ###
register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/FprimeFramer.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FprimeFramerTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FprimeFramerTester.cpp"
  UT_AUTO_HELPERS
)
