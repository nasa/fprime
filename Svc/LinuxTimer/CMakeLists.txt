####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(Linux Darwin)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	
else()
	
endif()

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/LinuxTimer.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/LinuxTimerCommon.cpp"
)

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/LinuxTimer.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/LinuxTimerTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/main.cpp"
)
