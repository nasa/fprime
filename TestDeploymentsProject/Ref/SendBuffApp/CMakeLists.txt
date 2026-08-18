####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
add_compile_options(
    -Wno-conversion
)
register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/SendBuffApp.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/SendBuffComponentImpl.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/SendBuff.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/SendBuffComponentImpl.hpp"
)
