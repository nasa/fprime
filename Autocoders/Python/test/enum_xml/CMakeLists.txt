# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Enum1EnumAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/Serial1SerializableAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/Port1PortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/Component1ComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/ExampleEnumImpl.cpp"
)
register_fprime_module()
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/main.cpp"
)
set(EXECUTABLE_NAME "Autocoders_Python_test_enum_xml_ut_exe")
set(MOD_DEPS
  "${CMAKE_CURRENT_LIST_DIR}")
register_fprime_executable()


