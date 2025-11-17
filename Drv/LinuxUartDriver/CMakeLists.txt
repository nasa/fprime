####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(Linux Darwin)

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/LinuxUartDriver.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/LinuxUartDriver.fpp"
  DEPENDS
    Os
)
