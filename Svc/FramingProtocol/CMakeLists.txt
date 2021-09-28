####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/DeframingProtocol.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/FramingProtocol.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/FprimeProtocol.cpp"
)

set(MOD_DEPS
    Fw/Buffer
    Fw/Time
    Utils/Hash
    Utils/Types
)

register_fprime_module()

