# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files

set(AUTOCODER_INPUT_FILES
  ""
)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ObjBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/SimpleObjRegistry.cpp"
)

set(LINK_DEPS
  Fw/Cfg
  Fw/Types
)

generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}" "${LINK_DEPS}")
