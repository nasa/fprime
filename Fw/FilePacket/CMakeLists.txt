####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####

# Note: no autocoding files, so basic dependencies must be hard-coded

set(FILE_PACKET_DEPS
    Fw/Types
    CFDP/Checksum
    Fw/Buffer
)
# For shared libraries, Os must exist in the module list
if (BUILD_SHARED_LIBS)
    list(APPEND FILE_PACKET_DEPS "Os")
endif()

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/CancelPacket.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/DataPacket.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/EndPacket.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FilePacket.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Header.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PathName.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/StartPacket.cpp"
  DEPENDS
    ${FILE_PACKET_DEPS}
)
# Add GTest directory
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/GTest")

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FilePacketMain.cpp"
  DEPENDS
    Fw/FilePacket/GTest
)
