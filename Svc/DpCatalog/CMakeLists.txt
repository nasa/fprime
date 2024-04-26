
####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/DpCatalog.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/DpCatalog.cpp"
)

set(HEADER_FILES
  "${CMAKE_CURRENT_LIST_DIR}/DpCatalog.hpp"
)


register_fprime_module()
### UTs ###
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/DpCatalog.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/DpCatalogTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/DpCatalogTesterHelpers.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/DpCatalogTestMain.cpp"
)
register_fprime_ut()
