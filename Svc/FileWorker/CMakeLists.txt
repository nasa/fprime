####
# FPrime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
# UT_SOURCE_FILES: list of source files for unit tests
#
# More information in the F´ CMake API documentation:
# https://fprime.jpl.nasa.gov/latest/docs/user-manual/cmake/cmake-api/
#
####

if (BUILD_TESTING AND NOT __FPRIME_NO_UT_GEN__)
    add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/FileTester")
endif()

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/FileWorker.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/FileWorker.cpp"
  DEPENDS
    Utils
)

register_fprime_ut(
    FileWorkerTestNominal
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/FileWorker.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FileWorkerTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FileWorkerTester.cpp"
  CHOOSES_IMPLEMENTATIONS
    Os_File
  DEPENDS
    STest
  UT_AUTO_HELPERS
  WORKING_DIRECTORY
    "${CMAKE_CURRENT_LIST_DIR}"
)

register_fprime_ut(
    FileWorkerTestError
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/FileWorker.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FileWorkerErrTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FileWorkerErrTester.cpp"
  CHOOSES_IMPLEMENTATIONS
    Svc_FileWorker_test_FileTester
  DEPENDS
    STest
  UT_AUTO_HELPERS
  WORKING_DIRECTORY
    "${CMAKE_CURRENT_LIST_DIR}"
)
