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
  "${CMAKE_CURRENT_LIST_DIR}/FilePacket.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Header.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/PathName.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/StartPacket.cpp"
)

set(LINK_DEPS
  Fw/Cfg
  Fw/Types
  CFDP/Checksum
  Fw/Comp
  Fw/Port
  Fw/Buffer
)

generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}" "${LINK_DEPS}")

# Sets MODULE_NAME to unique name based on path
get_module_name(${CMAKE_CURRENT_LIST_DIR})

# Depends on BufferSerializable defined in another module
add_dependencies(${MODULE_NAME} "Fw_Buffer")

# Add GTest directory
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/GTest")

# Add unit test directory
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/ut")
