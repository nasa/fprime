####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ComPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/ComPacket.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/ComBuffer.cpp"
)
register_fprime_module()
