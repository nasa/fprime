####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
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
)
register_fprime_module()
if (NOT FPRIME_FPP_LOCS_BUILD)
    get_module_name(${CMAKE_CURRENT_LIST_DIR})
    add_dependencies(${MODULE_NAME} version)
endif()
### UTs ###
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SystemResources.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
)
register_fprime_ut()
# Implementation requires switch cascade
if (TARGET ${MODULE_NAME}_ut_exe)
    target_compile_options(${MODULE_NAME}_ut_exe PRIVATE -Wno-implicit-fallthrough)
endif()

