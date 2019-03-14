# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files

set(AUTOCODER_INPUT_FILES
  "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentAi.xml"
)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentCommonImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentLinuxImpl.cpp"
)

generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}")
