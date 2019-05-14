####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(MOD_DEPS
    Fw/Cfg
    Fw/Types
)
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Logger.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/LogAssert.cpp"
)
register_fprime_module()
