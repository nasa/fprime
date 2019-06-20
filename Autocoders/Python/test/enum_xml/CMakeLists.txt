# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Enum1EnumAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/ExampleEnumSerializableAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/ExampleEnumPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/ExampleEnumComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/ExampleEnumImpl.cpp"
)
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/main.cpp"
)
register_fprime_module()
register_fprime_ut()


