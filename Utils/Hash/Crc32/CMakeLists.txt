####
# F prime CMakeLists.txt:
####

register_fprime_library(
    SOURCES
        Crc32.cpp
        HashImpl.cpp
    HEADERS
        Crc32.hpp
    DEPENDS
        Fw_Types
)

register_fprime_ut(
    SOURCES
        test/ut/Crc32Test.cpp
    DEPENDS
        Utils_Hash
)
