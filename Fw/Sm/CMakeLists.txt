####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/SmSignalBuffer.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/SmSignalBuffer.hpp"
  DEPENDS
    Fw_Types
)
