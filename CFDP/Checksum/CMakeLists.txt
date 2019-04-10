####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Checksum.cpp"
)

set(MOD_DEPS
  Fw/Cfg
  Fw/Types
)
register_fprime_module()

# Add GTest directory
##TODO-MSTARCH add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/GTest")

# Add unit test directory
##TODO-MSTARCH add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/ut")
