# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files

set(AUTOCODER_INPUT_FILES
  "${CMAKE_CURRENT_LIST_DIR}/CyclePortAi.xml"
)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/TimerVal.cpp"
)

set(LINK_DEPS
  Fw/Cfg
  Fw/Types
  Fw/Comp
  Fw/Port
  Os
)

generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}" "${LINK_DEPS}")
