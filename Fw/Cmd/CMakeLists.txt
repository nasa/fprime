####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(MOD_DEPS
    Fw/Com
)
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/CmdPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/CmdRegPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/CmdResponsePortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/CmdArgBuffer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/CmdPacket.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/CmdString.cpp"
)
register_fprime_module()
