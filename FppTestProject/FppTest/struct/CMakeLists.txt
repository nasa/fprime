# ====================================================================== 
# CMakeLists.txt
# ====================================================================== 

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/struct.fpp"
)

# Declare dependencies on test modules

# List all .cpp files as UT_SOURCE_FILES. Only the UT build is allowed.

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/PrimitiveStructTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/NonPrimitiveStructTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../utils/Utils.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
  DEPENDS
    Fw_Test
    STest
)
