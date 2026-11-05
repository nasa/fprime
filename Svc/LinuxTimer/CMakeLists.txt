####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(Linux Darwin)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  register_fprime_module(
    AUTOCODER_INPUTS
      "${CMAKE_CURRENT_LIST_DIR}/LinuxTimer.fpp"
    SOURCES
      "${CMAKE_CURRENT_LIST_DIR}/LinuxTimerCommon.cpp"
      "${CMAKE_CURRENT_LIST_DIR}/LinuxTimerTaskDelay.cpp"
    HEADERS
      "${CMAKE_CURRENT_LIST_DIR}/LinuxTimer.hpp"
  )
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  register_fprime_module(
    AUTOCODER_INPUTS
      "${CMAKE_CURRENT_LIST_DIR}/LinuxTimer.fpp"
    SOURCES
      "${CMAKE_CURRENT_LIST_DIR}/LinuxTimerCommon.cpp"
      "${CMAKE_CURRENT_LIST_DIR}/LinuxTimerFd.cpp"
    HEADERS
      "${CMAKE_CURRENT_LIST_DIR}/LinuxTimer.hpp"
  )
endif()

register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/LinuxTimer.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/LinuxTimerTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/main.cpp"
)
