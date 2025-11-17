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
    Fw/Com
)
### UTs ###

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/LogTest.cpp"
  DEPENDS
    "${FPRIME_FRAMEWORK_PATH}/Fw/Com"
    "${FPRIME_FRAMEWORK_PATH}/Fw/Obj"
    "${FPRIME_FRAMEWORK_PATH}/Fw/Port"
    "${FPRIME_FRAMEWORK_PATH}/Fw/Time"
    "${FPRIME_FRAMEWORK_PATH}/Fw/Types"
)
