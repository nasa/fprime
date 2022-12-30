####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/CancelPacket.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/DataPacket.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/EndPacket.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/FilePacket.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Header.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/PathName.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/StartPacket.cpp"
)
# Note: no autocoding files, so basic dependencies must be hard-coded
set(MOD_DEPS
  Fw/Cfg
  Fw/Types
  CFDP/Checksum
  Fw/Buffer
)
# For shared libraries, Os must exist in the module list
if (BUILD_SHARED_LIBS)
    list(APPEND MOD_DEPS "Os")
endif()
register_fprime_module()
# Add GTest directory
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/GTest")

set(UT_MOD_DEPS
    Fw/FilePacket/GTest
)

set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Main.cpp"
)
register_fprime_ut()
