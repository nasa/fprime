# Helper not available as fprime cmake code not loaded yet
if (NOT FPRIME_CMAKE_QUIET)
    message(STATUS "[F Prime] F Prime CMake package found at: ${CMAKE_CURRENT_LIST_FILE}")
endif()

# Ensure project() was called before find_package(FPrime)
if ("${PROJECT_SOURCE_DIR}" STREQUAL "" OR "${PROJECT_NAME}" STREQUAL "")
    message(FATAL_ERROR "[F Prime] find_package(FPrime) called before project(). "
        "Please call project() before find_package(FPrime) so that PROJECT_SOURCE_DIR is available.")
endif()

set(FPRIME_LOADED_VIA_FIND_PACKAGE TRUE CACHE INTERNAL "F Prime was loaded via find_package()")
include("${CMAKE_CURRENT_LIST_DIR}/FPrime.cmake")

# By default the F Prime package will load the codebase. This can be set OFF by setting the variable
# FPRIME_INCLUDE_FRAMEWORK_CODE to OFF. When set OFF, the user must call fprime_setup_included_code()
# to be able to use F Prime code. 
if ((NOT DEFINED FPRIME_INCLUDE_FRAMEWORK_CODE) OR (FPRIME_INCLUDE_FRAMEWORK_CODE))
    fprime_setup_included_code()    
endif()
