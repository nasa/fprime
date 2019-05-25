####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
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


# Add unit test directory
##TODO-MSTARCH add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/ut")

# Sets MODULE_NAME to unique name based on path
get_module_name(${CMAKE_CURRENT_LIST_DIR})

# Exclude test module from all build
set_target_properties(
  ${MODULE_NAME}
  PROPERTIES
  EXCLUDE_FROM_ALL TRUE
)
