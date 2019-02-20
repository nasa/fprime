# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files

set(AUTOCODER_INPUT_FILES
  "${CMAKE_CURRENT_LIST_DIR}/TlmPortAi.xml"
)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/TlmBuffer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TlmPacket.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TlmString.cpp"
)

generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}")

# Add unit test directory
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/ut")
