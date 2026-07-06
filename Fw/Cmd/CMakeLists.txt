####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Cmd.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/CmdPacket.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/CmdArgBuffer.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/CmdPacket.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/CmdString.hpp"
  DEPENDS
    Fw_Com
)
