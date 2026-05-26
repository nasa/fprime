# ====================================================================== 
# CMakeLists.txt
# ====================================================================== 

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/sizeof.fpp"
)


register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/sizeof.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
)
