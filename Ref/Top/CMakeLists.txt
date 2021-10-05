####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
####

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/instances.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/topology.fpp"
#  "${CMAKE_CURRENT_LIST_DIR}/RefTopologyAc.cpp"
#  "${CMAKE_CURRENT_LIST_DIR}/RefTopologyAppAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/RefTopologyDefs.cpp"
)
set(MOD_DEPS
  Fw/Logger
  Svc/LinuxTime
  # Communication Implementations
  Drv/Udp
  Drv/TcpClient
)

register_fprime_module()
get_module_name("${CMAKE_CURRENT_LIST_DIR}")
# Remove old autocoded C++ file from target
get_target_property(FINAL_SOURCE_FILES "${MODULE_NAME}" SOURCES) # TODO: fix this
list(REMOVE_ITEM FINAL_SOURCE_FILES "${CMAKE_CURRENT_BINARY_DIR}/RefTopologyAppAc.cpp")
set_target_properties("${MODULE_NAME}" PROPERTIES SOURCES "${FINAL_SOURCE_FILES}")
