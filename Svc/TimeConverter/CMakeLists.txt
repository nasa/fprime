####
# FPrime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
# UT_SOURCE_FILES: list of source files for unit tests
#
# More information in the F´ CMake API documentation:
# https://fprime.jpl.nasa.gov/latest/docs/user-manual/cmake/cmake-api/
#
####

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/TimeConverter.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/TimeConverter.cpp"
  DEPENDS
    Svc_Ports_TimeConverterPorts
)

register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/TimeConverter.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TimeConverterTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TimeConverterTestMain.cpp"
  UT_AUTO_HELPERS
)
