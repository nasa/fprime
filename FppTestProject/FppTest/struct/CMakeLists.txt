# ====================================================================== 
# CMakeLists.txt
# ====================================================================== 

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/struct.fpp"
)
register_fprime_module()

# Declare dependencies on test modules
set(UT_MOD_DEPS
  Fw/Test
  STest
)

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/../utils/Utils.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/EmptyTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/NonPrimitiveTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/PrimitiveTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
)
register_fprime_ut()
