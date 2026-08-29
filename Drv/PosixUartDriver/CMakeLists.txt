####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
# No restrict_platforms - POSIX compliant, available on all POSIX (Linux, Darwin, VxWorks, etc.)

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/PosixUartDriver.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/PosixUartDriver.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/PosixUartDriver.hpp"
  DEPENDS
    Os
)


