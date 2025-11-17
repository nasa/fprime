####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
# UT_SOURCE_FILES: list of source files for unit tests
#
####

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/FprimeRouter.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/FprimeRouter.fpp"
)

#### UTS ####

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FprimeRouterTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FprimeRouterTestMain.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/FprimeRouter.fpp"
  UT_AUTO_HELPERS
)
