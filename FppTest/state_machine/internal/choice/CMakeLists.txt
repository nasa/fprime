set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Basic.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/BasicU32.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/InputPairU16U32.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/JunctionToJunction.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/JunctionToState.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/Sequence.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/SequenceU32.fpp"
)
set(MOD_DEPS FppTest/state_machine/internal/harness)
register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Basic.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/BasicU32.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/InputPairU16U32.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/JunctionToJunction.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/JunctionToState.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Sequence.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/SequenceU32.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
)
set(UT_MOD_DEPS STest)
register_fprime_ut()
