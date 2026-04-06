####
# F prime CMakeLists.txt for Ports module:
#
# Contains port interface definitions and direct port calls support
#
####

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/DirectPortCalls.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/DirectPortCallsExample.cpp"
)

set(MOD_DEPS
  Fw/Types
  Fw/Obj
)

register_fprime_module()

