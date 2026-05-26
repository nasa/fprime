# ====================================================================== 
# CMakeLists.txt
# ====================================================================== 

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/struct.fpp"
)

# Declare dependencies on test modules

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/../utils/Utils.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/EmptyTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/NonPrimitiveTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PrimitiveTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
  DEPENDS
    Fw_Test
    STest
)
