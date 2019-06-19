# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ExampleEnumAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/ExampleEnumSerializableAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/ExampleEnumPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/ExampleEnumComponentAi.xml"
)

register_fprime_module()


