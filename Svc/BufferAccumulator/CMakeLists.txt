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
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/BufferAccumulator.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/BufferAccumulator.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ArrayFIFOBuffer.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/BufferAccumulator.hpp"
)

### UTS ###
register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/BufferAccumulator.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/BufferAccumulatorTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Accumulate.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Drain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Errors.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Health.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/BufferAccumulatorMain.cpp"
)

