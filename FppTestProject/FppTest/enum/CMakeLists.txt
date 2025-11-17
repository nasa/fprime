# ====================================================================== 
# CMakeLists.txt
# ====================================================================== 

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/implicit.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/explicit.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/default.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/interval.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/serialize_type.fpp"
)

# Declare dependencies on test modules

# List all .cpp files as UT_SOURCE_FILES. Only the UT build is allowed.

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/EnumToStringTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
  DEPENDS
    Fw/Test
    STest
)
