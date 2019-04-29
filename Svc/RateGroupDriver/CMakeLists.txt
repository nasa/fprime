####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/RateGroupDriverComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/RateGroupDriverImpl.cpp"
)

register_fprime_module()

### UTs ###
set(UT_SOURCE_FILES
  "${FPRIME_CORE_DIR}/Svc/RateGroupDriver/RateGroupDriverComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/RateGroupDriverTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/RateGroupDriverImplTester.cpp"
)
register_fprime_ut()
