####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
# UT_SOURCE_FILES: list of source files for unit tests
#
####

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Router.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/Router.cpp"
)
register_fprime_module()


#### UTS ####
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Router.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/RouterTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/RouterTestMain.cpp"
)
set(UT_AUTO_HELPERS ON)

register_fprime_ut()
