####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/SystemResources.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/SystemResources.cpp"
  DEPENDS
    Os
)
### UTs ###

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/SystemResources.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/SystemResourcesTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/SystemResourcesTestMain.cpp"
)
get_module_name("${CMAKE_CURRENT_LIST_DIR}")
if (TARGET ${MODULE_NAME}_ut_exe)
    target_compile_options(
        ${MODULE_NAME}_ut_exe
        PRIVATE
        -Wno-conversion
	# Implementation requires switch cascade
        -Wno-implicit-fallthrough
    )
endif()

