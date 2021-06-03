####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
if(NOT DEFINED FPRIME_USE_STUBBED_DRIVERS OR FPRIME_USE_STUBBED_DRIVERS)
    set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentAi.xml"
        "${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentImplCommon.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentImplStub.cpp"
    )
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "arm-linux-gnueabihf")
    set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentAi.xml"
        "${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentImplCommon.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverComponentImpl.cpp"
    )
else()
    message(FATAL_ERROR "Cannot use ${CMAKE_CURRENT_LIST_DIR} with platform ${CMAKE_SYSTEM_NAME}. Consider using -DFPRIME_USE_STUBBED_DRIVERS=ON")
endif()

register_fprime_module()
