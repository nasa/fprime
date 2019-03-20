# Default deployment cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files
# DEPLOYMNET_MODULES: Modules needed by this deployment

set(AUTOCODER_INPUT_FILES
  "${CMAKE_CURRENT_LIST_DIR}/RefTopologyAppAi.xml"
)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Topology.cpp"
)

set(DEPLOYMENT_MODULES
  Svc/PassiveConsoleTextLogger
  Svc/SocketGndIf
  Svc/LinuxTime
)

# invoke deployment function (DEPLOYMENT_NAME AUTOCODER_INPUT_FILES SOURCE_FILES DEPLOYMENT_MODULES)
generate_deployement("${PROJECT_NAME}" "${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}" "${DEPLOYMENT_MODULES}")
