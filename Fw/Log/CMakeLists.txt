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
  "${CMAKE_CURRENT_LIST_DIR}/LogBuffer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/LogPacket.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Log.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/LogString.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TextLogString.cpp"

)
register_fprime_module()
### UTs ###
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/LogTest.cpp"
)
set(UT_MOD_DEPS
  "${FPRIME_FRAMEWORK_PATH}/Fw/Com"
  "${FPRIME_FRAMEWORK_PATH}/Fw/Obj"
  "${FPRIME_FRAMEWORK_PATH}/Fw/Port"
  "${FPRIME_FRAMEWORK_PATH}/Fw/Time"
  "${FPRIME_FRAMEWORK_PATH}/Fw/Types"
)
register_fprime_ut()
