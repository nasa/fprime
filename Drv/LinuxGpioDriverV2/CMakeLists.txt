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
      AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverV2.fpp"
      SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverV2Common.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverV2Stub.cpp"
      HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverV2.hpp"
    )
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    register_fprime_module(
      AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverV2.fpp"
      SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverV2Common.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverV2.cpp"
      HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriverV2.hpp"
    )
endif()
