####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
add_compile_options(
    -Wno-conversion
)

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/SendBuffApp.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/SendBuffComponentImpl.cpp"
)
