####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/InputPortBase.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/InputSerializePort.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/OutputPortBase.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/OutputSerializePort.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PortBase.cpp"
  DEPENDS
    Fw/Types
    Fw/Logger
    Fw/Obj
)
