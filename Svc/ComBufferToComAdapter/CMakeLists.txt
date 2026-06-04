####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ComBufferToComAdapter.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/ComBufferToComAdapter.cpp"
)

register_fprime_module()
### UTs ###
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ComBufferToComAdapter.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComBufferToComAdapterTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComBufferToComAdapterTestMain.cpp"
)
register_fprime_ut()
