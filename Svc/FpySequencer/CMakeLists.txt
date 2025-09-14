####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
# UT_SOURCE_FILES: list of source files for unit tests
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/FpySequencer.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/FpySequencer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/FpySequencerTypes.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/FpySequencerStateMachine.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/FpySequencerDirectives.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/FpySequencerRunState.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/FpySequencerValidationState.cpp"
)

register_fprime_module()

### UTS ###
set(UT_AUTO_HELPERS ON)

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/FpySequencer.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/FpySequencer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/FpySequencerTypes.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/FpySequencerStateMachine.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/FpySequencerDirectives.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/FpySequencerRunState.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/FpySequencerValidationState.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/FpySequencerTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/FpySequencerTestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/FpySequencerTestSequences.cpp"
)

register_fprime_ut()
