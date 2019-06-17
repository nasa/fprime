####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####


if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	set(SOURCE_FILES
		"${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentAi.xml"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentImplCommon.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentImplStub.cpp"
	)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(SOURCE_FILES
		"${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentAi.xml"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentImplCommon.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentImpl.cpp"
	)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "CygWin")
	set(SOURCE_FILES
		"${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentAi.xml"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentImplCommon.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentImpl.cpp"
	)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "arm-linux-gnueabihf")
	set(SOURCE_FILES
		"${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentAi.xml"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentImplCommon.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentImpl.cpp"
	)
else()
	set(SOURCE_FILES
		"${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentAi.xml"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentImplCommon.cpp"
	)
endif()

register_fprime_module()
