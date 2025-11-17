####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/TlmBuffer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/TlmPacket.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Tlm.fpp"
  DEPENDS
    Fw_Com
)
### UTs ###

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TlmTest.cpp"
  DEPENDS
    Fw_Tlm
    Fw_Com
    Fw_Obj
    Fw_Port
    Fw_Time
    Fw_Types
)
