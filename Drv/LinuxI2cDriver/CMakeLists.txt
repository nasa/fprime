####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	set(SOURCE_FILES
		"${CMAKE_CURRENT_LIST_DIR}/LinuxI2cDriverComponentAi.xml"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxI2cDriverComponentImplStub.cpp"
	)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(SOURCE_FILES
		"${CMAKE_CURRENT_LIST_DIR}/LinuxI2cDriverComponentAi.xml"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxI2cDriverComponentImpl.cpp"
	)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "arm-linux-gnueabihf")
	set(SOURCE_FILES
		"${CMAKE_CURRENT_LIST_DIR}/LinuxI2cDriverComponentAi.xml"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxI2cDriverComponentImpl.cpp"
	)
else()
	set(SOURCE_FILES
		"${CMAKE_CURRENT_LIST_DIR}/LinuxI2cDriverComponentAi.xml"
	)
endif()

register_fprime_module()
