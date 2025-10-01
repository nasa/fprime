message(STATUS "[F Prime] F Prime CMake package found at: ${CMAKE_CURRENT_LIST_FILE}")
include("${CMAKE_CURRENT_LIST_DIR}/FPrime.cmake")

# By default the F Prime package will load the codebase. This can be set OFF by setting the variable
# FPRIME_INCLUDE_FRAMEWORK_CODE to OFF. When set OFF, the user must call fprime_setup_included_code()
# to be able to use F Prime code. 
if ((NOT DEFINED FPRIME_INCLUDE_FRAMEWORK_CODE) OR (FPRIME_INCLUDE_FRAMEWORK_CODE))
    fprime_setup_included_code()    
endif()
