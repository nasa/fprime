####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
add_compile_options(
    -Wno-conversion
    -Wno-double-promotion
    -Wno-shadow
)
set(HEADER_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SignalGen.hpp"
)

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/SignalGen.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/SignalGen.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/SignalGen.hpp"
)

### UTs ###
set(UT_HEADER_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SignalGenTester.hpp"
)
register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/SignalGen.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/SignalGenTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/SignalGenTestMain.cpp"
  UT_AUTO_HELPERS
)
