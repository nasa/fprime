restrict_platforms(Posix)

if (NOT FPRIME_IS_SUB_BUILD)
    message(DEPRECATION "Drv_LinuxUartDriver is deprecated; use Drv_PosixUartDriver")
endif()

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/LinuxUartDriver.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/LinuxUartDriver.hpp"
  DEPENDS
    Drv_PosixUartDriver
)
