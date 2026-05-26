####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(Linux Darwin)

set(PLATFORM_SOURCES)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  list(APPEND PLATFORM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/LinuxTimerTaskDelay.cpp")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  list(APPEND PLATFORM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/LinuxTimerFd.cpp")
endif()

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/LinuxTimer.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/LinuxTimerCommon.cpp"
    ${PLATFORM_SOURCES}
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/LinuxTimer.hpp"
)

register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/LinuxTimer.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/LinuxTimerTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/main.cpp"
)
