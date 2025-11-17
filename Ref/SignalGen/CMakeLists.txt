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

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/SignalGen.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/SignalGen.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/SignalGen.hpp"
)

### UTs ###

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/SignalGen.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/SignalGenTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/SignalGenTestMain.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/SignalGenTester.hpp"
  UT_AUTO_HELPERS
)
