####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
add_compile_options(
  -Wno-double-promotion
)

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/TypeDemo.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/TypeDemo.fpp"
  DEPENDS
    Os
)
