####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/SystemResources.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/SystemResources.cpp"
)
set(MOD_DEPS
  Os
  version
)
register_fprime_module()
### UTs ###
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SystemResources.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
)
register_fprime_ut()
get_module_name("${CMAKE_CURRENT_LIST_DIR}")
# Implementation requires switch cascade
if (TARGET ${MODULE_NAME}_ut_exe)
    target_compile_options(${MODULE_NAME}_ut_exe PRIVATE -Wno-implicit-fallthrough)
endif()

