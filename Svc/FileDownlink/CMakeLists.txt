####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/FileDownlink.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/FileDownlink.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/File.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Warnings.cpp"
)
set(MOD_DEPS
  Os
  Fw/FilePacket
  Utils/Hash
  CFDP/Checksum
)
register_fprime_module()
### UTs ###
set(UT_SOURCE_FILES
  "${FPRIME_FRAMEWORK_PATH}/Svc/FileDownlink/FileDownlink.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/FileDownlinkTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/FileDownlinkMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/FileBuffer.cpp"
)
register_fprime_ut()
