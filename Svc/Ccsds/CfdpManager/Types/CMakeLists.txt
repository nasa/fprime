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
        "${CMAKE_CURRENT_LIST_DIR}/Types.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/PduHeader.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/MetadataPdu.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/FileDataPdu.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/EofPdu.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/FinPdu.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/AckPdu.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/NakPdu.cpp"
    DEPENDS
        Svc_Ccsds_Types
)

### Unit Tests ###
register_fprime_ut(
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/PduTests.cpp"
)
