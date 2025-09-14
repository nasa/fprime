####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(MOD_DEPS
)
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/StatementArgBuffer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/StatementArgBuffer.fpp"
)
register_fprime_module()
