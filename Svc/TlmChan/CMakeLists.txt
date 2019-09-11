####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/TlmChanComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/TlmChanImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TlmChanImplGet.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TlmChanImplRecv.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TlmChanImplTask.cpp"
)

register_fprime_module()


### UTs ###
set(UT_SOURCE_FILES
  "${FPRIME_CORE_DIR}/Svc/TlmChan/TlmChanComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TlmChanTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TlmChanImplTester.cpp"
)
register_fprime_ut()

