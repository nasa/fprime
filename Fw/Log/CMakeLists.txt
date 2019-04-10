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
  "${CMAKE_CURRENT_LIST_DIR}/LogPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/LogTextPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/LogBuffer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/LogPacket.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/LogString.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TextLogString.cpp"

)
register_fprime_module()
### UTs ###
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/LogTest.cpp"
)
set(UT_MOD_DEPS
  "${FPRIME_CORE_DIR}/Fw/Com"
  "${FPRIME_CORE_DIR}/Fw/Obj"
  "${FPRIME_CORE_DIR}/Fw/Port"
  "${FPRIME_CORE_DIR}/Fw/Time"
  "${FPRIME_CORE_DIR}/Fw/Types"
)
register_fprime_ut()