# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files

set(AUTOCODER_INPUT_FILES
  "${CMAKE_CURRENT_LIST_DIR}/CmdPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/CmdRegPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/CmdResponsePortAi.xml"
)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/CmdArgBuffer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/CmdPacket.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/CmdString.cpp"
)

set(LINK_DEPS
  Fw/Cfg
  Fw/Types
  Fw/Comp
  Fw/Port
  Fw/Com
)

generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}" "${LINK_DEPS}")
