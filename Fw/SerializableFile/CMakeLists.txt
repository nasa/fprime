# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files

set(AUTOCODER_INPUT_FILES
  ""
)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SerializableFile.cpp"
)

generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}")

# Add TestSerializable module
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/TestSerializable")
# Add unit test directory
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/ut")
