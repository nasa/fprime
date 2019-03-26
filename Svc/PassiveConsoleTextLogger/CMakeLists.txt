# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files

set(AUTOCODER_INPUT_FILES
  ""
)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ConsoleTextLoggerImplCommon.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Stub/PrintfLoggerImplStub.cpp"
)

set(LINK_DEPS
  Fw/Cfg
  Fw/Types
  Fw/Comp
  Fw/Port
  Fw/Com
  Fw/Log
  Fw/Tlm
  Svc/PassiveTextLogger
)

generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}" "${LINK_DEPS}")

# Sets MODULE_NAME to unique name based on path
get_module_name(${CMAKE_CURRENT_LIST_DIR})

# Depends on a component defined in another module
add_dependencies(${MODULE_NAME} "Svc_PassiveTextLogger")
