####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
# UT_SOURCE_FILES: list of source files for unit tests
#
####

register_fprime_library(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/FrameAccumulator.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FrameDetector/FprimeFrameDetector.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FrameDetector/CcsdsTCFrameDetector.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/FrameAccumulator.fpp"
  DEPENDS
    Utils_Types
    Svc_FprimeProtocol
    Svc_CCSDS_Types
)

#### UTs ####
register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FrameAccumulatorTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FrameAccumulatorTestMain.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/FrameAccumulator.fpp"
  DEPENDS
    STest
  UT_AUTO_HELPERS
)

#### FrameDetector tests ####

register_fprime_ut(
  "Svc_FrameAccumulator_FprimeFrameDetector_test"
  SOURCES 
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/detectors/FprimeFrameDetectorTestMain.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/FrameDetector/FprimeFrameDetector.hpp"
  DEPENDS
    STest
  UT_AUTO_HELPERS
)

register_fprime_ut(
  "Svc_FrameAccumulator_CcsdsTCFrameDetector_test"
  SOURCES 
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/detectors/CcsdsTCFrameDetectorTestMain.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/FrameDetector/CcsdsTCFrameDetector.hpp"
  DEPENDS
    STest
  UT_AUTO_HELPERS
)
