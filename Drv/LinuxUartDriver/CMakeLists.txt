restrict_platforms(Posix)

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/LinuxUartDriver.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/LinuxUartDriver.hpp"
  DEPENDS
    Drv_PosixUartDriver
)
