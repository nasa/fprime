####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
register_fprime_module(
    FppTest_interfaces
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/output_ports.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/serial_ports_async.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/serial_ports.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/special_ports.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/typed_ports_async.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/typed_ports.fpp"
  INTERFACE
)
