
####
# F prime CMakeLists.txt:
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/DpCatalog.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/DpCatalog.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/DpCatalog.hpp"
)
### UTs ###
register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/DpCatalog.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/DpCatalogTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/DpCatalogTestMain.cpp"
  DEPENDS
    STest
  UT_AUTO_HELPERS
)

set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
  target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()

