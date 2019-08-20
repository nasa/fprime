####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####


if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	set(SOURCE_FILES
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSerialDriverComponentAi.xml"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSerialDriverComponentImplCommon.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSerialDriverComponentImplStub.cpp"
	)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(SOURCE_FILES
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSerialDriverComponentAi.xml"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSerialDriverComponentImplCommon.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSerialDriverComponentImpl.cpp"
	)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "CygWin")
	set(SOURCE_FILES
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSerialDriverComponentAi.xml"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSerialDriverComponentImplCommon.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSerialDriverComponentImpl.cpp"
	)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "arm-linux-gnueabihf")
	set(SOURCE_FILES
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSerialDriverComponentAi.xml"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSerialDriverComponentImplCommon.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSerialDriverComponentImpl.cpp"
	)
else()
	set(SOURCE_FILES
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSerialDriverComponentAi.xml"
		"${CMAKE_CURRENT_LIST_DIR}/LinuxSerialDriverComponentImplCommon.cpp"
	)
endif()

register_fprime_module()
