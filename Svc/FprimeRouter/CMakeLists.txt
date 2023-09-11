####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/FprimeRouter.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/FprimeRouter.cpp"
)

register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/FprimeRouter.fpp"
)
set(UT_MOD_DEPS STest)
#register_fprime_ut()
