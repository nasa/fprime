####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SignalGen.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/SignalGen.cpp"

)
set(HEADER_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SignalGen.hpp"
)

register_fprime_module()

### UTs ###
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SignalGen.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SignalGenTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SignalGenTestMain.cpp"
)
set(UT_HEADER_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SignalGenTester.hpp"
)
set(UT_AUTO_HELPERS ON)
register_fprime_ut()
