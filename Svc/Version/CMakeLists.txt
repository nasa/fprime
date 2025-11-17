####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
# UT_SOURCE_FILES: list of source files for unit tests
#
####

# Uncomment and add any modules that this component depends on, else
# they might not be available when cmake tries to build this component.

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Version.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/Version.cpp"
  DEPENDS
    Os
    version
)

set (UT_AUTO_HELPERS ON)
register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Version.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/version.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/VersionTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/VersionTestMain.cpp"
)
