####
# FPrime-Code:
#
# FPrime code. This cmake file includes the basic directories that make up the mainline F prime framework. This is
# separated from the CMake includes themselves such that this is built after all CMake setup. All F prime core folders
# should be added here and F prime CMake API function should be added elsewhere.
#
# Note: given F prime's historical folder structure, this is not organized as an fprime library.
####
# Libraries that make-up F prime. Hurray!
# Ignore GTest for non-test builds
if (${CMAKE_BUILD_TYPE} STREQUAL "TESTING")
    include("${FPRIME_FRAMEWORK_PATH}/cmake/googletest-download/googletest.cmake")
    add_subdirectory("${FPRIME_FRAMEWORK_PATH}/STest/" "${CMAKE_BINARY_DIR}/F-Prime/STest")
endif()
# By default we shutoff framework UTs
set(__FPRIME_NO_UT_GEN__ ON)
# Check for autocoder UTs
if (FPRIME_ENABLE_FRAMEWORK_UTS AND FPRIME_ENABLE_AUTOCODER_UTS)
    set(__FPRIME_NO_UT_GEN__ OFF)
endif()
add_subdirectory("${FPRIME_FRAMEWORK_PATH}/Autocoders/" "${CMAKE_BINARY_DIR}/F-Prime/Autocoders")
# Check if we are allowing framework UTs
if (FPRIME_ENABLE_FRAMEWORK_UTS)
    set(__FPRIME_NO_UT_GEN__ OFF)
endif()
add_subdirectory("${FPRIME_FRAMEWORK_PATH}/Fw/" "${CMAKE_BINARY_DIR}/F-Prime/Fw")
add_subdirectory("${FPRIME_FRAMEWORK_PATH}/Svc/" "${CMAKE_BINARY_DIR}/F-Prime/Svc")
add_subdirectory("${FPRIME_FRAMEWORK_PATH}/Os/" "${CMAKE_BINARY_DIR}/F-Prime/Os")
add_subdirectory("${FPRIME_FRAMEWORK_PATH}/Drv/" "${CMAKE_BINARY_DIR}/F-Prime/Drv")
add_subdirectory("${FPRIME_FRAMEWORK_PATH}/CFDP/" "${CMAKE_BINARY_DIR}/F-Prime/CFDP")
add_subdirectory("${FPRIME_FRAMEWORK_PATH}/Utils/" "${CMAKE_BINARY_DIR}/F-Prime/Utils")
# Always enable UTs for a project
set(__FPRIME_NO_UT_GEN__ OFF)
