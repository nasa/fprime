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

set(PLATFORM_SOURCES)
if(FPRIME_USE_STUBBED_DRIVERS)
    list(APPEND PLATFORM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriverComponentImplStub.cpp")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    list(APPEND PLATFORM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriverComponentImpl.cpp")
endif()

if(FPRIME_USE_STUBBED_DRIVERS OR ${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    register_fprime_module(
      AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriver.fpp"
      SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriverComponentImplCommon.cpp"
        ${PLATFORM_SOURCES}
      HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriver.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriverComponentImpl.hpp"
    )
endif()
