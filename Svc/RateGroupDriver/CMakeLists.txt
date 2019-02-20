# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files

set(AUTOCODER_INPUT_FILES
  "${CMAKE_CURRENT_LIST_DIR}/RateGroupDriverComponentAi.xml"
)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/RateGroupDriverImpl.cpp"
)

generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}")

# Add unit test directory
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/ut")
