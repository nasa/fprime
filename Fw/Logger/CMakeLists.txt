####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(MOD_DEPS
    Fw/Cfg
    Fw/Types
)
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Logger.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/LogAssert.cpp"
)
register_fprime_module()

# Rules based unit testing
set(UT_MOD_DEPS
    STest
    Fw/Types
)
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FakeLogger.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/LoggerRules.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Main.cpp"
)
# STest Includes for this UT
register_fprime_ut("Logger_Rules_Testing")
