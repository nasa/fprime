####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/PassiveComponentBase.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/ActiveComponentBase.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/PassiveComponentBase.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/QueuedComponentBase.hpp"
  DEPENDS
    Fw_Types
    Fw_Obj
    Fw_Port
)
# Makes active component its own library such that it can depend on Os where
# passive components do not.
register_fprime_module("Fw_CompQueued"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/QueuedComponentBase.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ActiveComponentBase.cpp"
  DEPENDS
    Fw_Types
    Fw_Obj
    Fw_Port
    Os
    Fw_Comp
)
