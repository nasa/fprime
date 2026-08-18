####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
# Note: no autocoding files, so basic dependencies must be hard-coded
register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/CancelPacket.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/DataPacket.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/EndPacket.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FilePacket.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Header.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PathName.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/StartPacket.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/FilePacket.hpp"
  DEPENDS
    Fw_Types
    CFDP_Checksum
    Fw_Buffer
    $<$<BOOL:${BUILD_SHARED_LIBS}>:Os>
)
# Add GTest directory
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/GTest")

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FilePacketMain.cpp"
  DEPENDS
    Fw_FilePacket_GTest
)
