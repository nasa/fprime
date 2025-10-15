set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/FppTestTopologyAc.cpp"
)

set(MOD_DEPS
  ${PROJECT_NAME}/topology/components/A
  ${PROJECT_NAME}/topology/components/B
)

register_fprime_module()
