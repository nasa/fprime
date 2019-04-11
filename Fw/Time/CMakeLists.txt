####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/TimePortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/Time.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Time.cpp"
)
register_fprime_module()
### UTs ###
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TimeTest.cpp"
)
set(UT_MOD_DEPS
  "${FPRIME_CORE_DIR}/Fw/Time"
  "${FPRIME_CORE_DIR}/Fw/Types"
)
register_fprime_ut()
