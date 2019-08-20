####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####


if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	set(SOURCE_FILES
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriverComponentAi.xml"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriverComponentImplCommon.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriverComponentImplStub.cpp"
	)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(SOURCE_FILES
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriverComponentAi.xml"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriverComponentImplCommon.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriverComponentImpl.cpp"
	)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "CygWin")
	set(SOURCE_FILES
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriverComponentAi.xml"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriverComponentImplCommon.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriverComponentImpl.cpp"
	)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "arm-linux-gnueabihf")
	set(SOURCE_FILES
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriverComponentAi.xml"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriverComponentImplCommon.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriverComponentImpl.cpp"
	)
else()
	set(SOURCE_FILES
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriverComponentAi.xml"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriverComponentImplCommon.cpp"
	)
endif()

register_fprime_module()
