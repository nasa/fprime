####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
if(FPRIME_USE_STUBBED_DRIVERS)
    set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriver.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentImplCommon.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentImplStub.cpp"
    )
    register_fprime_module()
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriver.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentImplCommon.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentImpl.cpp"
    )
    register_fprime_module()
else()
    message(STATUS "Cannot use ${CMAKE_CURRENT_LIST_DIR} with platform ${CMAKE_SYSTEM_NAME}. Skipping.")
endif()


