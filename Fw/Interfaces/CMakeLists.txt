####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
register_fprime_module(
    Fw_Interfaces
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Channel.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/Command.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/Event.fpp"
  INTERFACE
)
