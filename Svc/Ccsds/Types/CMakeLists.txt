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

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/config/SdlsKeyConfig")

register_fprime_library(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Types.fpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/SdlsKeyBuffer.hpp"
  DEPENDS
    Svc_Ccsds_Types_config_SdlsKeyConfig
)
