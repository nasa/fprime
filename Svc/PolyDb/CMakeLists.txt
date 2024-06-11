####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/PolyDb.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/PolyDb.cpp"
)

register_fprime_module()
### UTs ###
####
# WARNING:
#
# This UT does not use auto-generate GTest files. DO NOT emulate.
####

set(UT_AUTO_HELPERS ON)

set(UT_SOURCE_FILES
    "${FPRIME_FRAMEWORK_PATH}/Svc/PolyDb/PolyDb.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PolyDbTestMain.cpp"
#    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PolyDbTesterHelpers.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PolyDbTester.cpp"
)

register_fprime_ut()
