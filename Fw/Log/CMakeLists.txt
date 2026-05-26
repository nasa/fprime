####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Log.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/LogBuffer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/LogPacket.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/AmpcsEvrLogPacket.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/LogBuffer.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/LogPacket.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/LogString.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/TextLogString.hpp"
  DEPENDS
    Fw_Com
)
### UTs ###
register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/LogTest.cpp"
  DEPENDS
    Fw_Com
    Fw_Obj
    Fw_Port
    Fw_Time
    Fw_Types
)
