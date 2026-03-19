set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Framework.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Framework.fpp"
)

set(MOD_DEPS
   ${PROJECT_NAME}/topology/ports
)

register_fprime_module()
