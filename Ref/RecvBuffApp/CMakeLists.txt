####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/RecvBuffComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/PacketStatSerializableAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/RecvBuffComponentImpl.cpp"
)
register_fprime_module()
