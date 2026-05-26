####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/libcrc/CRC32.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/libcrc/lib_crc.c"
    "${CMAKE_CURRENT_LIST_DIR}/HashBufferCommon.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/HashCommon.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/Hash.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/HashBuffer.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/HashConfig.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/libcrc/CRC32.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/libcrc/lib_crc.h"
  DEPENDS
    Fw_Types
)
