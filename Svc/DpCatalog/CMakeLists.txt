####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/DpCatalog.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/DpCatalog.fpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/DpCatalog.hpp"
)
### UTs ###

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/DpCatalogTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/DpCatalogTesterHelpers.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/DpCatalogTestMain.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/DpCatalog.fpp"
  DEPENDS
    STest
)
set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()
