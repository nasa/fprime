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
    "${CMAKE_CURRENT_LIST_DIR}/FileManager.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/FileManager.cpp"
  DEPENDS
    Os
    Fw_FilePacket
    Utils_Hash
    CFDP_Checksum
)

register_fprime_ut(
  AUTOCODER_INPUTS
    "${FPRIME_FRAMEWORK_PATH}/Svc/FileManager/FileManager.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FileManagerTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FileManagerMain.cpp"
  UT_AUTO_HELPERS
)
