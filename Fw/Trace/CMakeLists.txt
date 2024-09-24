####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(MOD_DEPS
    Fw/Com
)
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/TraceBuffer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TraceEntity.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Trace.fpp"
)
register_fprime_module()