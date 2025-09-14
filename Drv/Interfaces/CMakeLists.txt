####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
register_fprime_module(
    Drv_Interfaces
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/AsyncByteStreamDriver.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/ByteStreamDriver.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/Gpio.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/I2c.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/Spi.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/Tick.fpp"
  INTERFACE
)
