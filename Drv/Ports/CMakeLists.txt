####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/GpioDriverPorts.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/I2cDriverPorts.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/SpiDriverPorts.fpp"
)

register_fprime_module()
