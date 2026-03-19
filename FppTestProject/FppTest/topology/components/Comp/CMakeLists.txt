set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Comp.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Comp.fpp"
)

set(MOD_DEPS
   ${PROJECT_NAME}/topology/ports
)

register_fprime_module()
