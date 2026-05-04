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
    "${CMAKE_CURRENT_LIST_DIR}/SpacePacketDeframer.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/SpacePacketDeframer.fpp"
  DEPENDS
    Svc_Ccsds_Types
)

# Allow comparison of a configurable constant with a fixed-width constant that may be "always true" in
# specific configurations buy may not be true in other configurations. Relates to: FwSizeType
target_compile_options(${FPRIME_CURRENT_MODULE} PRIVATE -Wno-tautological-constant-out-of-range-compare)

### Unit Tests ###
register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/SpacePacketDeframerTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/SpacePacketDeframerTester.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/SpacePacketDeframer.fpp"
  DEPENDS
    Svc_Ccsds_Types
    STest
  UT_AUTO_HELPERS
)

