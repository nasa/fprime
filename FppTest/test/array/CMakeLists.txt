# ====================================================================== 
# CMakeLists.txt
# ====================================================================== 

# We need to declare the XML source files this way to invoke the autocoder.
# However, only the UT build is allowed here.
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/array.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/enum.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/format.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/string.fpp"
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
  "${CMAKE_CURRENT_LIST_DIR}/StringArrayTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/ArrayArrayTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/EnumArrayTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/StructArrayTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/StringTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/FormatTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../utils/utils.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
)
register_fprime_ut()
