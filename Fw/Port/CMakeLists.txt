# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files

set(AUTOCODER_INPUT_FILES
  ""
)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/InputPortBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/InputSerializePort.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/OutputPortBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/OutputSerializePort.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/PortBase.cpp"
)

set(LINK_DEPS
  Fw/Cfg
  Fw/Obj
  Fw/Types
  Os
)

generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}" "${LINK_DEPS}")
