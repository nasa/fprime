####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Checksum.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/Checksum.hpp"
  DEPENDS
    Fw_Types
)


register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ChecksumMain.cpp"
  DEPENDS
    CFDP_Checksum
    Fw_Types
)
# Add GTest directory
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/GTest")
