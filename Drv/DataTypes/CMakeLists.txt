####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/DataTypes.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/DataBuffer.cpp"
)

set(MOD_DEPS
    Fw/Port
)

register_fprime_module()
