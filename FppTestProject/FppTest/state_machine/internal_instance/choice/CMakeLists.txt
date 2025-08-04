set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Basic.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/BasicU32.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/ChoiceToChoice.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/ChoiceToState.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/InputPairU16U32.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/Sequence.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/SequenceU32.fpp"
)
set(MOD_DEPS FppTest/state_machine/internal/harness)
register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/BasicTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/BasicU32Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/ChoiceToChoiceTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/ChoiceToStateTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/InputPairU16U32Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/SequenceTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/SequenceU32Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
)
set(UT_MOD_DEPS STest)
register_fprime_ut()
