# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files

set(AUTOCODER_INPUT_FILES
  ""
)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ActiveComponentBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/PassiveComponentBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/QueuedComponentBase.cpp"
)

set(LINK_DEPS
  Fw/Cfg
  Fw/Obj
  Fw/Types
  Fw/Port
)

generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}" "${LINK_DEPS}")
