# ====================================================================== 
# CMakeLists.txt
# ====================================================================== 

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/sizeof.fpp"
)
register_fprime_module()


set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/sizeof.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
)
register_fprime_ut()
