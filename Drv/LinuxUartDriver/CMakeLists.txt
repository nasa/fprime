####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(MOD_DEPS Os)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/LinuxUartDriver.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/LinuxUartDriver.cpp"
    )
    register_fprime_module()
else()
    message(STATUS "Cannot use ${CMAKE_CURRENT_LIST_DIR} with platform ${CMAKE_SYSTEM_NAME}. Skipping.")
endif()
