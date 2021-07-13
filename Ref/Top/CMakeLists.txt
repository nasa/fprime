####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Main.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/RefTopologyAc.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/RefTopologyAppAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/RefTopologyDefs.cpp"
)
set(MOD_DEPS
  Fw/Logger
  Svc/LinuxTime
  # Communication Implementations
  Drv/Udp
  Drv/TcpClient
)
register_fprime_executable()
# Remove old autocoded C++ file from target
get_target_property(FINAL_SOURCE_FILES Ref_exe SOURCES)
list(REMOVE_ITEM FINAL_SOURCE_FILES "${CMAKE_CURRENT_BINARY_DIR}/RefTopologyAppAc.cpp")
set_target_properties(
   Ref_exe
   PROPERTIES
   SOURCES "${FINAL_SOURCE_FILES}"
)
