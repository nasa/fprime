####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/PrmGetPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/PrmSetPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/PrmBuffer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/PrmString.cpp"
)

register_fprime_module()

