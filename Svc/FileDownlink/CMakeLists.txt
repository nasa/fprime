####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/FileDownlink.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/FileDownlink.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/File.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Warnings.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/FileDownlink.hpp"
  DEPENDS
    Os
    Fw_FilePacket
    Utils_Hash
    CFDP_Checksum
)
### UTs ###
register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/FileDownlink.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FileDownlinkTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FileDownlinkMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FileBuffer.cpp"
)
set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()
