# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Component1ComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/Enum1EnumAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/Port1PortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/Serial1SerializableAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/ExampleEnumImpl.cpp"
)
register_fprime_module()


## Add unit test directory
## UT_SOURCE_FILES: Sources for unit test
#set(UT_SOURCE_FILES
#  "${CMAKE_CURRENT_LIST_DIR}/test/ut/main.cpp"
#)
#register_fprime_ut()

