####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
restrict_platforms(Linux Darwin) # Uses sys/time

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/BufferAccumulator.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ArrayFIFOBuffer.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/BufferAccumulator.fpp"
)

### UTS ###

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/BufferAccumulatorTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Accumulate.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Drain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Errors.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Health.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/BufferAccumulatorMain.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/BufferAccumulator.fpp"
)

