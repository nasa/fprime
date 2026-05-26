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
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/TcpServer.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/TcpServerComponentImpl.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/TcpServer.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/TcpServerComponentImpl.hpp"
  DEPENDS
    Fw_Logger
    Drv_ByteStreamDriverModel
    Drv_Ip
)

### UTs ###
register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/TcpServer.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TcpServerTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TcpServerTester.cpp"
  DEPENDS
    STest
    SocketTestHelper
  UT_AUTO_HELPERS
)
set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()
