####
# F Prime CMakeLists.txt:
#
# SOURCES: list of source files (to be compiled)
# AUTOCODER_INPUTS: list of files to be passed to the autocoders
# DEPENDS: list of libraries that this module depends on
#
# More information in the F´ CMake API documentation:
# https://fprime.jpl.nasa.gov/latest/docs/reference/api/cmake/API/
#
####

register_fprime_library(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/ComRetry.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/ComRetry.cpp"
    DEPENDS
        Fw_Types
        Fw_Buffer
)

### UTs ###
register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComRetryTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComRetryTester.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/ComRetry.fpp"
  UT_AUTO_HELPERS
)

set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()
