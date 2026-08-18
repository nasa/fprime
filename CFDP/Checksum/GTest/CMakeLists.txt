####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
if (BUILD_TESTING)
    register_fprime_module(
      SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Checksums.cpp"
      HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/Checksums.hpp"
      DEPENDS
        gtest
        CFDP_Checksum
    )
endif()
