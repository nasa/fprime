####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(Posix SOCKETS)

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/IpSocket.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/TcpClientSocket.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/TcpServerSocket.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UdpSocket.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SocketComponentHelper.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/IpSocket.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/SocketComponentHelper.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/TcpClientSocket.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/TcpServerSocket.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/UdpSocket.hpp"
  DEPENDS
    Os
    Fw_Buffer
)

# The PortSelector library and testing helper is only needed on Testing build
if (BUILD_TESTING)
    add_library(SocketTestHelper STATIC
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/PortSelector.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/SocketTestHelper.cpp")
    target_include_directories(SocketTestHelper PUBLIC
            "${FPRIME_FRAMEWORK_PATH}/STest"
            "${FPRIME_FRAMEWORK_PATH}/googletest/googletest/include"
    )
    add_dependencies(SocketTestHelper STest Fw_Buffer)
    target_link_libraries(SocketTestHelper STest Fw_Buffer)
endif()


### UTs ###
register_fprime_ut(Drv_Ip_Tcp_test
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestTcp.cpp"
  DEPENDS
    SocketTestHelper
)

register_fprime_ut(Drv_Ip_Udp_test
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestUdp.cpp"
  DEPENDS
    SocketTestHelper
)

