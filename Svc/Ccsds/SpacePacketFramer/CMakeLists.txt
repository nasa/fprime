####
# FPrime CMakeLists.txt:
#
# SOURCES: list of source files (to be compiled)
# AUTOCODER_INPUTS: list of files to be passed to the autocoders
# DEPENDS: list of libraries that this module depends on
#
# More information in the F´ CMake API documentation:
# https://fprime.jpl.nasa.gov/latest/documentation/reference
#
####

register_fprime_library(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/SpacePacketFramer.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/SpacePacketFramer.fpp"
  DEPENDS
    Svc_Ccsds_Types
)


### Unit Tests ###
register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/SpacePacketFramerTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/SpacePacketFramerTester.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/SpacePacketFramer.fpp"
  DEPENDS
    STest
  UT_AUTO_HELPERS
)