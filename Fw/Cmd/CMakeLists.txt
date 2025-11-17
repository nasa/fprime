####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/CmdArgBuffer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CmdPacket.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Cmd.fpp"
  DEPENDS
    Fw_Com
)
