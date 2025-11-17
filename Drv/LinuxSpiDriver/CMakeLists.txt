####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
if (NOT FPRIME_USE_STUBBED_DRIVERS)
    restrict_platforms(Linux)
endif()
if(FPRIME_USE_STUBBED_DRIVERS)
    register_fprime_module(
        SOURCES
            "${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriver.fpp"
            "${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriverComponentImplCommon.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriverComponentImplStub.cpp"
    )
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    register_fprime_module(
        SOURCES
            "${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriver.fpp"
            "${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriverComponentImplCommon.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriverComponentImpl.cpp"
    )
endif()

