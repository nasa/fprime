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
  "${CMAKE_CURRENT_LIST_DIR}/PolyDbImpl.cpp"
)

register_fprime_module()
### UTs ###
####
# WARNING:
#
# This UT does not use auto-generate GTest files. DO NOT emulate.
####
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/PolyDbComponentTestAc.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/PolyDbTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/PolyDbImplTester.cpp"
)
register_fprime_ut()
