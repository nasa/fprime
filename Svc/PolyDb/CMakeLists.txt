####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/PolyDb.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/PolyDb.fpp"
)
### UTs ###
####
# WARNING:
#
# This UT does not use auto-generate GTest files. DO NOT emulate.
####

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PolyDbTestMain.cpp"
    #    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PolyDbTesterHelpers.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PolyDbTester.cpp"
  AUTOCODER_INPUTS
    "${FPRIME_FRAMEWORK_PATH}/Svc/PolyDb/PolyDb.fpp"
  UT_AUTO_HELPERS
)
