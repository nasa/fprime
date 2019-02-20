# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files

set(AUTOCODER_INPUT_FILES
  "${CMAKE_CURRENT_LIST_DIR}/BufferGetPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/BufferSendPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/BufferSerializableAi.xml"
)

set(SOURCE_FILES
  ""
)

generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}")
