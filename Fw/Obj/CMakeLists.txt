####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ObjBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/SimpleObjRegistry.cpp"
)
set(MOD_DEPS
  Fw/Cfg
  Fw/Types
  Fw/Logger
)

register_fprime_module()
