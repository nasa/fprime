####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
	"${CMAKE_CURRENT_LIST_DIR}/ExampleComponentAi.xml"
	"${CMAKE_CURRENT_LIST_DIR}/ExamplePortAi.xml"
	"${CMAKE_CURRENT_LIST_DIR}/AnotherPortAi.xml"
	"${CMAKE_CURRENT_LIST_DIR}/ExampleSerializableAi.xml"
	"${CMAKE_CURRENT_LIST_DIR}/Example2SerializableAi.xml"
	"${CMAKE_CURRENT_LIST_DIR}/ExampleComponentImpl.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/ExampleType.cpp"
)

register_fprime_module()
##
# Excludes this target from all, preventing it from being built unless needed.
##
get_module_name(${CMAKE_CURRENT_LIST_DIR})
set_target_properties(
  ${MODULE_NAME}
  PROPERTIES
  EXCLUDE_FROM_ALL TRUE
)

set(UT_SOURCE_FILES
  "${FPRIME_FRAMEWORK_PATH}/Autocoders/Python/templates/ExampleComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/main.cpp"
)
register_fprime_ut()
