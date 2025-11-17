####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
# UT_SOURCE_FILES: list of source files for unit tests
#
####

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/FpySequencer.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/FpySequencer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FpySequencerTypes.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/FpySequencerStateMachine.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FpySequencerDirectives.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FpySequencerRunState.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FpySequencerValidationState.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FpySequencerStack.cpp"
)

### UTS ###

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/FpySequencer.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/FpySequencer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FpySequencerTypes.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/FpySequencerStateMachine.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FpySequencerDirectives.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FpySequencerRunState.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FpySequencerValidationState.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FpySequencerStack.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FpySequencerTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FpySequencerTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FpySequencerTestSequences.cpp"
  UT_AUTO_HELPERS
)
