####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
# UT_SOURCE_FILES: list of source files for unit tests
#
####

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/FprimeRouter.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/FprimeRouter.cpp"
)
register_fprime_module()


#### UTS ####
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/FprimeRouter.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/FprimeRouterTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/FprimeRouterTestMain.cpp"
)
set(UT_AUTO_HELPERS ON)

register_fprime_ut()
