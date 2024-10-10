####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
# UT_SOURCE_FILES: list of source files for unit tests
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SeqDispatcher.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/SeqDispatcher.cpp"
)

register_fprime_module()

### UTS ###
set(UT_AUTO_HELPERS ON)

set(UT_SOURCE_FILES
  "${FPRIME_FRAMEWORK_PATH}/Svc/SeqDispatcher/SeqDispatcher.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SeqDispatcherTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SeqDispatcherTestMain.cpp"
)
register_fprime_ut()
