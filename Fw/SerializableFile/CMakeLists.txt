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
  DEPENDS
    Fw/Types
    Os
)
### UTs ###

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Test.cpp"
  DEPENDS
    Fw/SerializableFile
    Fw/SerializableFile/test/TestSerializable
    Fw/Types
    Utils/Hash
    Os
)
