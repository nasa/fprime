# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files)

set(AUTOCODER_INPUT_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Simple_ActiveComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/Simple_Active_TesterComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/Active_TesterTopologyAppAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/F32PortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/U32PortAi.xml"
)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Simple_ActiveImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Simple_Active_TesterImpl.cpp"
)

set(DEPLOYMENT_MODULES
  ""
)

# # invoke deployment function (DEPLOYMENT_NAME AUTOCODER_INPUT_FILES SOURCE_FILES DEPLOYMENT_MODULES)
# generate_deployement("Active_Tester" "${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}" "${DEPLOYMENT_MODULES}")
#
# # Exclude test executable from all build
# set_target_properties(
#   "Active_Tester"
#   PROPERTIES
#   EXCLUDE_FROM_ALL TRUE
# )

generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}")

# Sets MODULE_NAME to unique name based on path
get_module_name(${CMAKE_CURRENT_LIST_DIR})

# Exclude test module from all build
set_target_properties(
  ${MODULE_NAME}
  PROPERTIES
  EXCLUDE_FROM_ALL TRUE
)

# Add unit test directory
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/ut")
