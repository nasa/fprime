# ====================================================================== 
# CMakeLists.txt
# ====================================================================== 

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/alias.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/array.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/component.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/enum.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/state_machine.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/string.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/struct.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/format.fpp"
)

# Declare dependencies on test modules

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ArrayToStringTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FormatTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../utils/Utils.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
  DEPENDS
    Fw_Test
    STest
)
