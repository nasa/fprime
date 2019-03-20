# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files

set(AUTOCODER_INPUT_FILES
  "${CMAKE_CURRENT_LIST_DIR}/PrmGetPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/PrmSetPortAi.xml"
)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/PrmBuffer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/PrmString.cpp"
)

set(LINK_DEPS
  Fw/Cfg
  Fw/Types
  Fw/Comp
  Fw/Port
)

generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}" "${LINK_DEPS}")
