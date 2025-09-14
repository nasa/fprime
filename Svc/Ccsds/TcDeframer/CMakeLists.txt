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
    "${CMAKE_CURRENT_LIST_DIR}/TcDeframer.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/TcDeframer.fpp"
  DEPENDS
    Svc_Ccsds_Types
)

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TcDeframerTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TcDeframerTester.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/TcDeframer.fpp"
  DEPENDS
    Svc_Ccsds_Types
    STest
  UT_AUTO_HELPERS
)
