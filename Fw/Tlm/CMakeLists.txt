####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Tlm.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/TlmPacket.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/TlmBuffer.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/TlmPacket.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/TlmString.hpp"
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
