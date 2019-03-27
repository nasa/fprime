# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files

set(AUTOCODER_INPUT_FILES
  ""
)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/libcrc/CRC32.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/libcrc/lib_crc.c"
  "${CMAKE_CURRENT_LIST_DIR}/HashBufferCommon.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/HashCommon.cpp"
)

generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}" "Fw/Types")
