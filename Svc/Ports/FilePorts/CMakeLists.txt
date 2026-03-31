####
# CMakeLists.txt:
#
# Sets up the fprime module build within CMake.
####
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/FileAnnounce.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/FileDispatch.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/FileWorkerPorts.fpp"
)
register_fprime_module()
