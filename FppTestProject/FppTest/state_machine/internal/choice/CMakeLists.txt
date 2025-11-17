

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Basic.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/BasicU32.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/ChoiceToChoice.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/ChoiceToState.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/InputPairU16U32.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/Sequence.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/SequenceU32.fpp"
  DEPENDS
    FppTest_state_machine_internal_harness
)

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Basic.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/BasicU32.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ChoiceToChoice.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ChoiceToState.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/InputPairU16U32.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Sequence.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SequenceU32.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
  DEPENDS
    STest
)
