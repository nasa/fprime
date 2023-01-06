####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/PingReceiver.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/PingReceiverComponentImpl.cpp"
)
register_fprime_module()
