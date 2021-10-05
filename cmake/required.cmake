####
# required.cmake:
#
# Required tools for the fprime CMake system.
####

find_program(FPUTIL NAMES fprime-util REQUIRED)
find_program(PYTHON NAMES python3 python REQUIRED)
find_program(JAVA java PATHS ENV JAVA_HOME ENV PATH REQUIRED)

# Check python was found
if (NOT FPUTIL)
    message(FATAL_ERROR "fprime-util was not found. Please install with 'pip install fprime-tools'")
elseif (NOT PYTHON)
    message(FATAL_ERROR "python3 was not found. Please ensure python3 is on the path.")
elseif(NOT JAVA)
    message(FATAL_ERROR "java was not found. Please ensure java is on the path.")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/fpp-download/fpp.cmake")
