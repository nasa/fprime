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

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/RawTimeTester")

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/OsTime.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/OsTime.cpp"
)

set(MOD_DEPS
    Svc_Ports_OsTimeEpoch
)

register_fprime_module()

set(UT_AUTO_HELPERS ON)
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/OsTime.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/OsTimeTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/OsTimeTestMain.cpp"
)
set(UT_MOD_DEPS
    Svc_OsTime_test_RawTimeTester
)
choose_fprime_implementation(Os/RawTime Svc_OsTime_test_RawTimeTester)
register_fprime_ut()
