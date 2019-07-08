####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ActiveComponentBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/PassiveComponentBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/QueuedComponentBase.cpp"
)
# Note: no autocoding files, so basic includes must be hard-coded
set(MOD_DEPS
  Fw/Cfg
  Fw/Types
  Fw/Obj
  Fw/Port
)
# For shared libraries, Os must exist in the module list
if (BUILD_SHARED_LIBS)
    list(APPEND MOD_DEPS "Os")
endif()
register_fprime_module()
