####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/HealthComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/HealthComponentImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Stub/HealthComponentStubChecks.cpp"
)

register_fprime_module()
##TODO-MSTARCH: add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/ut")
