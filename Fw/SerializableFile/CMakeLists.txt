####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
if (BUILD_TESTING)
    add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/TestSerializable")
endif()

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/SerializableFile.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/SerializableFile.hpp"
  DEPENDS
    Fw_Types
    Os
)

### UTs ###
register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Test.cpp"
  DEPENDS
    Fw_SerializableFile
    Fw_SerializableFile_test_TestSerializable
    Fw_Types
    Utils_Hash
    Os
)
