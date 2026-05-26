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
)
register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/RecvBuffApp.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/RecvBuffComponentImpl.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/RecvBuff.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/RecvBuffComponentImpl.hpp"
  DEPENDS
    Os
    Fw_Cmd
)
