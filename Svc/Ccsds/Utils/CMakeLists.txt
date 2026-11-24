####
# F Prime CMakeLists.txt:
#
# SOURCES: list of source files (to be compiled)
# AUTOCODER_INPUTS: list of files to be passed to the autocoders
# DEPENDS: list of libraries that this module depends on
#
# More information in the F´ CMake API documentation:
# https://fprime.jpl.nasa.gov/devel/docs/reference/api/cmake/API/
#
####

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/AesGcm/")

register_fprime_library(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/IdlePacket.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/CRC16.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/IdlePacket.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/SdlsAad.hpp"
  DEPENDS
    Svc_Ccsds_Types
)
