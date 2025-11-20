set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Sender.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Sender.fpp"
)
set(MOD_DEPS
  Fw/Comp
  Fw/Types
  Os
  ${PROJECT_NAME}/topology/ports
)
register_fprime_module()

target_link_libraries(FppTest_topology_components_Sender PRIVATE gtest)
