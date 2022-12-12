####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/RPITopologyDefs.cpp"  
  "${CMAKE_CURRENT_LIST_DIR}/instances.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/topology.fpp"
)
# Note: supply non-explicit dependencies here
set(MOD_DEPS
   Fw/Logger
   Svc/LinuxTime
   # Communication Implementations
   Drv/Udp
   Drv/TcpClient
)

register_fprime_module()
