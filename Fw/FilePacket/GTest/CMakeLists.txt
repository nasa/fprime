# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files

set(AUTOCODER_INPUT_FILES
  ""
)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/CancelPacket.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/DataPacket.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/EndPacket.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Header.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/PathName.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/StartPacket.cpp"
)

set(LINK_DEPS
  Fw/Cfg
  Fw/Types
  Fw/Comp
  Fw/Port
)
generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}" "${LINK_DEPS}")

# Add dependency on gtest
get_module_name(${CMAKE_CURRENT_LIST_DIR})
add_dependencies(${MODULE_NAME} ${GTEST_TARGET}) 
