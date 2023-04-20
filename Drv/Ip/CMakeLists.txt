####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
	"${CMAKE_CURRENT_LIST_DIR}/IpSocket.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/TcpClientSocket.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/TcpServerSocket.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/UdpSocket.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/SocketReadTask.cpp"
)

set(MOD_DEPS
	Os
	Fw/Buffer
)

register_fprime_module()

# The PortSelector library and testing helper is only needed on Testing build
if (BUILD_TESTING)
	add_library(PortSelector STATIC
		"${CMAKE_CURRENT_LIST_DIR}/test/ut/PortSelector.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/test/ut/SocketTestHelper.cpp")
	target_include_directories(PortSelector PUBLIC
			"${FPRIME_FRAMEWORK_PATH}/STest"
			"${FPRIME_FRAMEWORK_PATH}/gtest/googletest-src/googletest/include"
	)
	add_dependencies(PortSelector STest Fw_Buffer)
	target_link_libraries(PortSelector STest Fw_Buffer)
endif()

set(UT_MOD_DEPS
	PortSelector
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

