####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
####
add_compile_options(
  -Wno-shadow
)

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/instances.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/topology.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/RefTopology.cpp"
  DEPENDS
    Fw_Logger
    Svc_PosixTime
)
