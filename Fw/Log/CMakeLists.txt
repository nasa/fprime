####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/LogBuffer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/LogPacket.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Log.fpp"
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
