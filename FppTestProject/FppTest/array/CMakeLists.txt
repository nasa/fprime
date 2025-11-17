# ====================================================================== 
# CMakeLists.txt
# ====================================================================== 

# We need to declare the FPP source files this way to invoke the autocoder.
# However, only the UT build is allowed here.

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/alias.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/array.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/enum.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/string.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/struct.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/format.fpp"
)

# Declare dependencies on test modules

# List all .cpp files as UT_SOURCE_FILES. Only the UT build is allowed.

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ArrayToStringTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FormatTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../utils/Utils.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
  DEPENDS
    Fw/Test
    STest
)
