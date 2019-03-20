# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files

set(AUTOCODER_INPUT_FILES
  "${CMAKE_CURRENT_LIST_DIR}/PolyDbComponentAi.xml"
)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/PolyDbImpl.cpp"
)

set(LINK_DEPS
  Fw/Cfg
  Fw/Types
  Fw/Comp
  Fw/Port
)

generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}" "${LINK_DEPS}")

# Add unit test directory
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/ut")
