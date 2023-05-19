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
if (BUILD_TESTING AND NOT DEFINED FPRIME_PRESCAN)
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
# Faux libraries used as interfaces to non-autocoded fpp items
add_library(Fpp INTERFACE)
set(FPRIME_CURRENT_MODULE config)
add_subdirectory("${FPRIME_CONFIG_DIR}" "${CMAKE_BINARY_DIR}/config")
set(_FP_CORE_PACKAGES Fw Svc Os Drv CFDP Utils)
foreach (_FP_PACKAGE_DIR IN LISTS _FP_CORE_PACKAGES)
    set(FPRIME_CURRENT_MODULE "${_FP_PACKAGE_DIR}")
    add_subdirectory("${FPRIME_FRAMEWORK_PATH}/${_FP_PACKAGE_DIR}/" "${CMAKE_BINARY_DIR}/F-Prime/${_FP_PACKAGE_DIR}")
endforeach ()
unset(FPRIME_CURRENT_MODULE)
# Always enable UTs for a project
set(__FPRIME_NO_UT_GEN__ OFF)
foreach (LIBRARY_DIR IN LISTS FPRIME_LIBRARY_LOCATIONS)
    file(GLOB MANIFESTS RELATIVE "${LIBRARY_DIR}" CONFIGURE_DEPENDS "${LIBRARY_DIR}/*.cmake")
    foreach (MANIFEST IN LISTS MANIFESTS)
        include("${LIBRARY_DIR}/${MANIFEST}")
    endforeach()
endforeach()
