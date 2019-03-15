# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files

set(AUTOCODER_INPUT_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ComPortAi.xml"
)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ComPacket.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/ComBuffer.cpp"
)

generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}")
