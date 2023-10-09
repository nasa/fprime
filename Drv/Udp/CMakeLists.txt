####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
	"${CMAKE_CURRENT_LIST_DIR}/Udp.fpp"
	"${CMAKE_CURRENT_LIST_DIR}/UdpComponentImpl.cpp"
)

# Necessary shared helpers
set(MOD_DEPS
	"Fw/Logger"
	"Drv/ByteStreamDriverModel"
	"Drv/Ip"
)

register_fprime_module()

### UTs ###
set(UT_SOURCE_FILES
	"${CMAKE_CURRENT_LIST_DIR}/Udp.fpp"
	"${CMAKE_CURRENT_LIST_DIR}/test/ut/UdpTestMain.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/test/ut/UdpTester.cpp"
)
set(UT_MOD_DEPS
	STest
	PortSelector
)
set(UT_AUTO_HELPERS ON)
register_fprime_ut()
