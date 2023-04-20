####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(MOD_DEPS
  "${CMAKE_THREAD_LIBS_INIT}" 
  Fw/Cfg
  Fw/Types
  Fw/Logger
  Utils/Hash
)
set(SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/TaskRunner.cpp")
register_fprime_module()