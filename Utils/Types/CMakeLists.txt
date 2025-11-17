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
    "Fw/Types"
)

# Rules based unit testing
set(UT_MOD_DEPS
    STest
    Fw/Types
)
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/CircularBuffer/CircularState.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/CircularBuffer/CircularRules.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/CircularBuffer/CircularBufferTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/CircularBuffer/Main.cpp"
)
# STest Includes for this UT
set (UT_TARGET_NAME "Types_Circular_Buffer_ut_exe")
register_fprime_ut("${UT_TARGET_NAME}")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()
