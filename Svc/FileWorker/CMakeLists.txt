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

# FileTester is a test dependency and only supported on Posix platforms, therefore:
# - only attempt to add the FileTester module in BUILD_TESTING
# - only add the FileWorkerTestError test if the FileTester module was successfully added
if (BUILD_TESTING AND NOT __FPRIME_NO_UT_GEN__)
    add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/FileTester")

    # Target will not exist on non-Posix platforms, so guard against its existence
    if (TARGET Svc_FileWorker_test_FileTester)
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
    else()
      # Only print message if we're in BUILD_TESTING *and* FileTester isn't available
      message(STATUS "Svc/FileWorker: FileTester test dependency is only supported on Posix, skipping FileWorkerTestError")
    endif()
endif()
