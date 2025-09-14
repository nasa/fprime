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

# List all .cpp files as UT_SOURCE_FILES. Only the UT build is allowed.
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/PrimitiveStructTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/NonPrimitiveStructTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../utils/Utils.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
)
register_fprime_ut()
