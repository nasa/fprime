####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####

register_fprime_library(
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/HashBufferCommon.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/HashCommon.cpp"

        "${CMAKE_CURRENT_LIST_DIR}/Crc32/Crc32.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Crc32/HashImpl.cpp"

        # Only provided to satisfy CCSDS
        "${CMAKE_CURRENT_LIST_DIR}/libcrc/lib_crc.c"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/Hash.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/HashBuffer.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/HashConfig.hpp"

        "${CMAKE_CURRENT_LIST_DIR}/Crc32/Crc32.hpp"

        # Only provided to satisfy CCSDS
        "${CMAKE_CURRENT_LIST_DIR}/libcrc/lib_crc.h"
    DEPENDS
        Fw_Types
)

register_fprime_ut(
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Crc32/test/ut/Crc32Test.cpp"
    DEPENDS
        Utils_Hash
)
