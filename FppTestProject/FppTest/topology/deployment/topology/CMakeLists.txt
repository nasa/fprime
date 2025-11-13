set(SOURCE_FILES
  # "${CMAKE_CURRENT_LIST_DIR}/FppTestTopologyAc.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/instances.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/topology.fpp"
)

set(MOD_DEPS
  ${PROJECT_NAME}/topology/components/Sender
  ${PROJECT_NAME}/topology/components/Receiver
)

register_fprime_module()
