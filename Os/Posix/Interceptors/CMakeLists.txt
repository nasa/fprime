####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
# On testing build only
if (NOT BUILD_TESTING)
    return()
endif()

set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/FileStub.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../FileSystem.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../Directory.cpp"
)
set(MOD_DEPS
  Fw/Cfg
  Fw/Types
)
register_fprime_module(Os_File_PosixWithInterceptors)
target_include_directories(Os_File_PosixWithInterceptors PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
register_fprime_implementation(Os_File Os_File_PosixWithInterceptors)

