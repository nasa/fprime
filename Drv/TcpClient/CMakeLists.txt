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
    "${CMAKE_CURRENT_LIST_DIR}/TcpClient.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/TcpClientComponentImpl.cpp"
  DEPENDS
    "Fw/Logger"
    "Drv/ByteStreamDriverModel"
    "Drv/Ip"
)

### UTs ###

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/TcpClient.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TcpClientTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TcpClientTester.cpp"
  DEPENDS
    STest
    SocketTestHelper
  UT_AUTO_HELPERS
)
set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()
