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
    Svc_CCSDS_Types
)


### Unit Tests ###
# set(UT_SOURCE_FILES
#   "${CMAKE_CURRENT_LIST_DIR}/SpacePacketFramer.fpp"
#   "${CMAKE_CURRENT_LIST_DIR}/test/ut/SpacePacketFramerTestMain.cpp"
#   "${CMAKE_CURRENT_LIST_DIR}/test/ut/SpacePacketFramerTester.cpp"
# )
# set(UT_MOD_DEPS
#   STest
# )
# set(UT_AUTO_HELPERS ON)
# register_fprime_ut()
