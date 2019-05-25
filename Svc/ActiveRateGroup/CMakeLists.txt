####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ActiveRateGroupComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/ActiveRateGroupImpl.cpp"
)

register_fprime_module()
### UTs ###
set(UT_SOURCE_FILES
  "${FPRIME_CORE_DIR}/Svc/ActiveRateGroup/ActiveRateGroupComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ActiveRateGroupTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ActiveRateGroupImplTester.cpp"
)
register_fprime_ut()
