####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(MOD_DEPS
    Fw/Com
)
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/CmdArgBuffer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/CmdPacket.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Cmd.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/CmdString.cpp"
)
register_fprime_module()
