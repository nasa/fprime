####
# F Prime CMakeLists.txt:
#
# SOURCES: list of source files (to be compiled)
# AUTOCODER_INPUTS: list of files to be passed to the autocoders
# DEPENDS: list of libraries that this module depends on
#
# More information in the F´ CMake API documentation:
# https://fprime.jpl.nasa.gov/latest/docs/reference/api/cmake/API/
#
####

register_fprime_library(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/CfdpManager.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/CfdpManager.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Engine.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Chunk.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Clist.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Utils.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Timer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Channel.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/TransactionTx.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/TransactionRx.cpp"
    DEPENDS
        CFDP_Checksum
        Svc_Ccsds_CfdpManager_Types
)

### Subdirectories ###
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Types/")

### Unit Tests ###
register_fprime_ut(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/CfdpManager.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/CfdpManagerTestMain.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/CfdpManagerTester.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/PduTester.cpp"
    UT_AUTO_HELPERS
)
