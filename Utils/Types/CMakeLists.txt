####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/CircularBuffer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Queue.cpp"
  DEPENDS
    Fw_Types
)

# Rules based unit testing
register_fprime_ut(
  "Types_Circular_Buffer_ut_exe"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/CircularBuffer/CircularState.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/CircularBuffer/CircularRules.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/CircularBuffer/CircularBufferTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/CircularBuffer/Main.cpp"
  DEPENDS
    STest
    Fw_Types
)
# STest Includes for this UT
set (UT_TARGET_NAME "Types_Circular_Buffer_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()
