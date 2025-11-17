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
    "${CMAKE_CURRENT_LIST_DIR}/FprimeRouter.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/FprimeRouter.cpp"
)

#### UTS ####

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/FprimeRouter.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FprimeRouterTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FprimeRouterTestMain.cpp"
  UT_AUTO_HELPERS
)
