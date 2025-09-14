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
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ApidManager.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/ApidManager.fpp"
)



### Unit Tests ###
register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ApidManagerTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ApidManagerTester.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/ApidManager.fpp"
  DEPENDS
    Svc_Ccsds_Types
    STest
  UT_AUTO_HELPERS
)
