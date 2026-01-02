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
        "${CMAKE_CURRENT_LIST_DIR}/CfdpTypes.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/CfdpCfg.fpp"
        SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/CfdpManager.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/CfdpEngine.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/CfdpCodec.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/CfdpRx.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/CfdpTx.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/CfdpChunk.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/CfdpClist.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/CfdpUtils.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/CfdpDispatch.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/CfeStubs.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/CfdpTimer.cpp"
        # TODO This should be moved to the F' config directory
        "${CMAKE_CURRENT_LIST_DIR}/CfdpCfg.hpp"
)

### Unit Tests ###
# register_fprime_ut(
#     AUTOCODER_INPUTS
#         "${CMAKE_CURRENT_LIST_DIR}/CfdpManager.fpp"
#     SOURCES
#         "${CMAKE_CURRENT_LIST_DIR}/test/ut/CfdpManagerTestMain.cpp"
#         "${CMAKE_CURRENT_LIST_DIR}/test/ut/CfdpManagerTester.cpp"
#     DEPENDS
#         STest # For rules-based testing
#     UT_AUTO_HELPERS
# )
