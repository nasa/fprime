####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Crc32")

register_fprime_library(
    SOURCES
        HashBufferCommon.cpp
        HashCommon.cpp
        # Only provided to satisfy CCSDS
        libcrc/lib_crc.c
    HEADERS
        Hash.hpp
        HashBuffer.hpp
        HashConfig.hpp
        libcrc/lib_crc.h
    DEPENDS
        Fw_Types
        Utils_Hash_Crc32
)
