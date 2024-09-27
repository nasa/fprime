####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/File.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/Task.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/Mutex.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/Directory.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/FileSystem.fpp"
)
register_fprime_module()
