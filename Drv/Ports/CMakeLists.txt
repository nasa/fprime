####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/DataTypes/")


register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/GpioDriverPorts.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/I2cDriverPorts.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/SpiDriverPorts.fpp"
)
