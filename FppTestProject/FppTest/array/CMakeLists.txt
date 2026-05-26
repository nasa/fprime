# ====================================================================== 
# CMakeLists.txt
# ====================================================================== 

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/alias.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/array.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/component.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/enum.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/state_machine.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/string.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/struct.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/format.fpp"
)
register_fprime_module()

# Declare dependencies on test modules
set(UT_MOD_DEPS
  Fw/Test
  STest
)

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ArrayToStringTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/FormatTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../utils/Utils.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
)
register_fprime_ut()
