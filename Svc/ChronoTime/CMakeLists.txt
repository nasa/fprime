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

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ChronoTime.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/ChronoTime.cpp"
)

register_fprime_module()

set(UT_AUTO_HELPERS ON)
set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/ChronoTime.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/ChronoTimeTester.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/ChronoTimeTestMain.cpp"
)
register_fprime_ut()