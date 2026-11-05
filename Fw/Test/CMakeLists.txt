####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/UnitTestAssert.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/String.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTest.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTestAssert.hpp"
  DEPENDS
    Fw_Types
)
