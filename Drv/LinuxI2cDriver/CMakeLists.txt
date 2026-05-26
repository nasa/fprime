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
    add_definitions(-DSTUBBED_LINUX_I2C_DRIVER)
    list(APPEND PLATFORM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/LinuxI2cDriverStub.cpp")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    list(APPEND PLATFORM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/LinuxI2cDriver.cpp")
endif()

if(FPRIME_USE_STUBBED_DRIVERS OR ${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    register_fprime_module(
      AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/LinuxI2cDriver.fpp"
      SOURCES
        ${PLATFORM_SOURCES}
      HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/LinuxI2cDriver.hpp"
    )
endif()
