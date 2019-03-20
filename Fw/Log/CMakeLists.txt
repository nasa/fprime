# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files

set(AUTOCODER_INPUT_FILES
  "${CMAKE_CURRENT_LIST_DIR}/LogPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/LogTextPortAi.xml"
)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/LogBuffer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/LogPacket.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/LogString.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TextLogString.cpp"
)

set(LINK_DEPS
  Fw/Cfg
  Fw/Types
  Fw/Comp
  Fw/Port
  Fw/Com
)

generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}" "${LINK_DEPS}")

# Add unit test directory
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/ut")
