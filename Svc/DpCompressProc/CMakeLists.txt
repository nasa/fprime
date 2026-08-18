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

# Module names are derived from the path from the nearest project/library/framework
# root when not specifically overridden by the developer, i.e. the module defined by
# `MyProj/Some/Path/CMakeLists.txt` will be named `MyProj_Some_Path`.

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Types")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Ports")

register_fprime_library(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/DpCompressProc.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/DpCompressProc.cpp"
#   DEPENDS
#       MyPackage_MyOtherModule
)


register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/DpCompressProc.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/AbstractState.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/DpCompressProcTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/DpCompressProcTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/ProcRequest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/Testers.cpp"
    #"${CMAKE_CURRENT_LIST_DIR}/test/ut/Scenarios/Random.cpp"
  DEPENDS
    STest
  UT_AUTO_HELPERS
)
