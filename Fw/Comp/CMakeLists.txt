####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####

# Note: no autocoding files, so basic includes must be hard-coded

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/PassiveComponentBase.cpp"
  DEPENDS
    Fw/Types
    Fw/Obj
    Fw/Port
)
# Makes active component its own library such that it can depend on Os where
# passive components do not.
list(APPEND MOD_DEPS Os Fw/Comp)

register_fprime_module("Fw_CompQueued")
