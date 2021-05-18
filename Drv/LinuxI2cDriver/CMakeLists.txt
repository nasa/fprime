####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
if(NOT DEFINED FPRIME_USE_STUBBED_DRIVERS OR FPRIME_USE_STUBBED_DRIVERS)
    add_definitions(-DSTUBBED_LINUX_I2C_DRIVER)
    set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/LinuxI2cDriverComponentAi.xml"
        "${CMAKE_CURRENT_LIST_DIR}/LinuxI2cDriverComponentImplStub.cpp"
    )
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "arm-linux-gnueabihf")
    set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/LinuxI2cDriverComponentAi.xml"
        "${CMAKE_CURRENT_LIST_DIR}/LinuxI2cDriverComponentImpl.cpp"
    )
else()
    message(FATAL_ERROR "Cannot use ${CMAKE_CURRENT_LIST_DIR} with platform ${CMAKE_SYSTEM_NAME}. Consider using -DFPRIME_USE_STUBBED_DRIVERS=ON")
endif()

register_fprime_module()
