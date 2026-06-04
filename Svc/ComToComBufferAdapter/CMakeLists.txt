####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ComToComBufferAdapter.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/ComToComBufferAdapter.cpp"
)

register_fprime_module()
### UTs ###
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ComToComBufferAdapter.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComToComBufferAdapterTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComToComBufferAdapterTestMain.cpp"
)
register_fprime_ut()
