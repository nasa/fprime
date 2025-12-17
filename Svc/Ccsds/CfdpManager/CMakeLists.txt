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
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/CfdpManager.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/cf_codec.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/cf_cfdp.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/cf_cfdp_r.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/cf_cfdp_s.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/cf_chunk.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/cf_clist.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/CfeStubs.hpp"
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
