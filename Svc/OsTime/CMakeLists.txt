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

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/RawTimeTester")

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/OsTime.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/OsTime.cpp"
  DEPENDS
    Svc_Ports_OsTimeEpoch
)

register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/OsTime.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/OsTimeTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/OsTimeTestMain.cpp"
  CHOOSES_IMPLEMENTATIONS
    Svc_OsTime_test_RawTimeTester
  UT_AUTO_HELPERS
)
