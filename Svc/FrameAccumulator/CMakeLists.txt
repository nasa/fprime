####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
# UT_SOURCE_FILES: list of source files for unit tests
#
####

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/FrameAccumulator.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/FrameAccumulator.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/FrameDetector/FprimeFrameDetector.cpp"
)
set(MOD_DEPS
    Utils/Types
    Svc/FprimeProtocol
)

register_fprime_module()

#### UTs ####
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/FrameAccumulator.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/FrameAccumulatorTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/FrameAccumulatorTestMain.cpp"
)
set(UT_MOD_DEPS
    Utils/Types
    STest
)
set(UT_AUTO_HELPERS ON)

register_fprime_ut()

#### FprimeFrameDetector tests ####

set(UT_HEADER_FILES
  "${CMAKE_CURRENT_LIST_DIR}/FrameDetector/FprimeFrameDetector.hpp"
)
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/detectors/FprimeFrameDetectorTestMain.cpp"
)

register_fprime_ut("Svc_FrameAccumulator_FprimeFrameDetector_test")
