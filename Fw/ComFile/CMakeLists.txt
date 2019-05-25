####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/FileBuffer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/FilePacket.cpp"
)
set(MOD_DEPS
  Fw/Cfg
  Fw/Types
)
register_fprime_module()
