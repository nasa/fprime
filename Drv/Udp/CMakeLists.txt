####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(Posix SOCKETS)

# Necessary shared helpers

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/UdpComponentImpl.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Udp.fpp"
  DEPENDS
    Fw_Logger
    Drv_ByteStreamDriverModel
    Drv_Ip
)

### UTs ###

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/UdpTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/UdpTester.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Udp.fpp"
  DEPENDS
    STest
    SocketTestHelper
  UT_AUTO_HELPERS
)
set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()
