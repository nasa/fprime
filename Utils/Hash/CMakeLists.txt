####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####

register_fprime_library(
    SOURCES
        HashBufferCommon.cpp
        HashCommon.cpp

        Crc32/Crc32.cpp
        Crc32/HashImpl.cpp

        # Only provided to satisfy CCSDS
        libcrc/lib_crc.c
    HEADERS
        Hash.hpp
        HashBuffer.hpp
        HashConfig.hpp

        Crc32/Crc32.hpp

        # Only provided to satisfy CCSDS
        libcrc/lib_crc.h
    DEPENDS
        Fw_Types
)

register_fprime_ut(
    SOURCES
        Crc32/test/ut/Crc32Test.cpp
    DEPENDS
        Utils_Hash
)
