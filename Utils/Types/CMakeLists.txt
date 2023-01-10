####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/CircularBuffer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Queue.cpp"
)
set(MOD_DEPS
  "Fw/Types"
)
register_fprime_module()

# Rules based unit testing
set(UT_MOD_DEPS
    STest
    Fw/Types
)
set(UT_SOURCE_FILES
    "test/ut/CircularBuffer/CircularState.cpp"
    "test/ut/CircularBuffer/CircularRules.cpp"
    "test/ut/CircularBuffer/Main.cpp"
)
# STest Includes for this UT
register_fprime_ut("Types_Circular_Buffer_ut_exe")
