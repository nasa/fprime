####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
	"${CMAKE_CURRENT_LIST_DIR}/../ByteStreamDriverModel/ByteStreamDriverComponentAi.xml"
	"${CMAKE_CURRENT_LIST_DIR}/TcpServerComponentImpl.cpp"
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
	"${CMAKE_CURRENT_LIST_DIR}/../ByteStreamDriverModel/ByteStreamDriverComponentAi.xml"
	"${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
)
set(UT_MOD_DEPS
	STest
	PortSelector
)
register_fprime_ut()
