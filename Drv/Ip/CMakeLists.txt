####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(Posix)

set(SOURCE_FILES
	"${CMAKE_CURRENT_LIST_DIR}/IpSocket.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/TcpClientSocket.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/TcpServerSocket.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/UdpSocket.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/SocketComponentHelper.cpp"
)

set(MOD_DEPS
	Os
	Fw/Buffer
)

register_fprime_module()

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

set(UT_MOD_DEPS
	SocketTestHelper
)

### UTs ###
set(UT_SOURCE_FILES
	"${CMAKE_CURRENT_LIST_DIR}/test/ut/TestTcp.cpp"
)
register_fprime_ut("Drv_Ip_Tcp_test")

set(UT_SOURCE_FILES
	"${CMAKE_CURRENT_LIST_DIR}/test/ut/TestUdp.cpp"
)
register_fprime_ut("Drv_Ip_Udp_test")

