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
  "${CMAKE_CURRENT_LIST_DIR}/TlmPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/TlmBuffer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TlmPacket.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TlmString.cpp"
)
register_fprime_module()
### UTs ###
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TlmTest.cpp"
)
set(UT_MOD_DEPS
  "${FPRIME_CORE_DIR}/Fw/Tlm"
  "${FPRIME_CORE_DIR}/Fw/Com"
  "${FPRIME_CORE_DIR}/Fw/Obj"
  "${FPRIME_CORE_DIR}/Fw/Port"
  "${FPRIME_CORE_DIR}/Fw/Time"
  "${FPRIME_CORE_DIR}/Fw/Types"
)
register_fprime_ut()