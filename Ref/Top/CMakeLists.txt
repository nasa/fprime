# Default deployment cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files
# DEPLOYMNET_MODULES: Modules needed by this deployment

set(AUTOCODER_INPUT_FILES
  "${CMAKE_CURRENT_LIST_DIR}/RefTopologyAppAi.xml"
)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Topology.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/StubTargetInit.cpp"
)

set(DEPLOYMENT_MODULES
  # Reference components
  "${CMAKE_CURRENT_LIST_DIR}/../PingReceiver"
  "${CMAKE_CURRENT_LIST_DIR}/../RecvBuffApp"
  "${CMAKE_CURRENT_LIST_DIR}/../SendBuffApp"
  "${CMAKE_CURRENT_LIST_DIR}/../SignalGen"

  # Services componets
  Svc/ActiveLogger
  Svc/ActiveRateGroup
  Svc/AssertFatalAdapter
  Svc/BufferManager
  Svc/CmdDispatcher
  Svc/CmdSequencer
  Svc/FatalHandler
  Svc/FileDownlink
  Svc/FileUplink
  Svc/GndIf
  Svc/Health
  Svc/LinuxTime
  Svc/PassiveConsoleTextLogger
  Svc/PassiveTextLogger
  Svc/PrmDb
  Svc/RateGroupDriver
  Svc/SocketGndIf
  Svc/Time
  Svc/TlmChan

  # Services ports
  Svc/Cycle
  Svc/Fatal
  Svc/Ping
  Svc/Sched
  Svc/Seq
  Svc/WatchDog

  # Driver Ports
  Drv/DataTypes

  # Driver Components
  Drv/BlockDriver

  # CFDP modules
  CFDP/Checksum

  # Utility modules
  Utils/Hash

  # Framework ports
  Fw/Buffer
  Fw/Cmd
  Fw/Log
  Fw/Time
  Fw/Tlm
  Fw/Prm
  Fw/Com

  # Framework
  Fw/FilePacket
  Fw/Comp
  Fw/Obj
  Fw/Port
  Fw/Types
  Os
)

# invoke deployment function (DEPLOYMENT_NAME AUTOCODER_INPUT_FILES SOURCE_FILES DEPLOYMENT_MODULES)
generate_deployement("Ref" "${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}" "${DEPLOYMENT_MODULES}")
