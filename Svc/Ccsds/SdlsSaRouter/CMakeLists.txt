####
# FPrime CMakeLists.txt:
#
# SOURCES: list of source files (to be compiled)
# AUTOCODER_INPUTS: list of files to be passed to the autocoders
# DEPENDS: list of libraries that this module depends on
#
# More information in the F´ CMake API documentation:
# https://fprime.jpl.nasa.gov/devel/docs/reference/api/cmake/API/
#
####

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/config/SdlsSaRouterConfig")

register_fprime_library(
  EXCLUDE_FROM_ALL
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/SdlsSaRouter.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/SdlsSaRouter.fpp"
  DEPENDS
    Svc_Ccsds_SdlsSaRouter_config_SdlsSaRouterConfig
)

### Unit Tests ###
register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/SdlsSaRouterTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/SdlsSaRouterTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestState/TestState.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/Route.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/DataFlow.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/SdlsSaRouter.fpp"
  DEPENDS
    Svc_Ccsds_Types
    Svc_Ccsds_SdlsSaRouter_config_SdlsSaRouterConfig
    STest
  UT_AUTO_HELPERS
)
