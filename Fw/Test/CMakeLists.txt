####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/String.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/UnitTestAssert.cpp"
)
set(MOD_DEPS Fw_Types)
register_fprime_module()
