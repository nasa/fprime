####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/PassiveComponentBase.cpp"
)
# Note: no autocoding files, so basic includes must be hard-coded
set(MOD_DEPS
  Fw/Cfg
  Fw/Types
  Fw/Obj
  Fw/Port
)
register_fprime_module()
# Makes active component its own library such that it can depend on Os where
# passive components do not.
list(APPEND MOD_DEPS Os Fw/Comp)
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/QueuedComponentBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/ActiveComponentBase.cpp"
)
register_fprime_module("Fw_CompQueued")
