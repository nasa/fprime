####
# FPrime CMakeLists.txt:
#
# SOURCES: list of source files (to be compiled)
# AUTOCODER_INPUTS: list of files to be passed to the autocoders
# DEPENDS: list of libraries that this module depends on
#
# More information in the F´ CMake API documentation:
# https://fprime.jpl.nasa.gov/devel/docs/reference/api/cmake/API/
#
####

register_fprime_library(
  EXCLUDE_FROM_ALL
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ClearTextDecryptor.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/ClearTextDecryptor.fpp"
)

### Unit Tests ###
register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ClearTextDecryptorTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ClearTextDecryptorTester.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/ClearTextDecryptor.fpp"
  DEPENDS
    STest
  UT_AUTO_HELPERS
)
