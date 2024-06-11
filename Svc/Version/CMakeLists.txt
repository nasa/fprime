####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
# UT_SOURCE_FILES: list of source files for unit tests
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Version.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/Version.cpp"
)

# Uncomment and add any modules that this component depends on, else
# they might not be available when cmake tries to build this component.

set(MOD_DEPS
   Os
   version 
)

register_fprime_module()

set(UT_SOURCE_FILES
   "${CMAKE_CURRENT_LIST_DIR}/Version.fpp"
   "${CMAKE_CURRENT_LIST_DIR}/test/ut/VersionTester.cpp"
   "${CMAKE_CURRENT_LIST_DIR}/test/ut/VersionTestMain.cpp"
)
set (UT_AUTO_HELPERS ON)
register_fprime_ut()
