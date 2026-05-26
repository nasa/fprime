# ====================================================================== 
# CMakeLists.txt
# ====================================================================== 

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/component.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/default.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/explicit.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/implicit.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/interval.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/serialize_type.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/state_machine.fpp"
)

# Declare dependencies on test modules

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/EnumToStringTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
  DEPENDS
    Fw_Test
    STest
)
