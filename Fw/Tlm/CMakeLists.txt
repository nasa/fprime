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
  "${CMAKE_CURRENT_LIST_DIR}/Tlm.fpp"
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
  "${FPRIME_FRAMEWORK_PATH}/Fw/Tlm"
  "${FPRIME_FRAMEWORK_PATH}/Fw/Com"
  "${FPRIME_FRAMEWORK_PATH}/Fw/Obj"
  "${FPRIME_FRAMEWORK_PATH}/Fw/Port"
  "${FPRIME_FRAMEWORK_PATH}/Fw/Time"
  "${FPRIME_FRAMEWORK_PATH}/Fw/Types"
)
register_fprime_ut()
