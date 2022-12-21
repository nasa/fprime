####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/libcrc/CRC32.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/libcrc/lib_crc.c"
  "${CMAKE_CURRENT_LIST_DIR}/HashBufferCommon.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/HashCommon.cpp"
)
set(MOD_DEPS
  "Fw/Types"
)
register_fprime_module()