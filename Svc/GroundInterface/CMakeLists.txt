####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/GroundInterfaceComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/GroundInterface.cpp"
)
set(MOD_DEPS "Utils/Types")
register_fprime_module()
