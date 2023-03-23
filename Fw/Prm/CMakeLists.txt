####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Prm.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/PrmBuffer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/PrmString.cpp"
)

register_fprime_module()

