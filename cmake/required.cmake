####
# required.cmake:
#
# Required tools for the fprime CMake system. This system pre-checks all the tools needed to generate and build through
# cmake. These items are strictly required and will fail if not found, which will prevent confusing errors further
# within the build process
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

message(STATUS "[python3] python3 found at: ${PYTHON}")
message(STATUS "[fpp-tools] fpp-depend found at: ${FPP_DEPEND}")
message(STATUS "[fpp-tools] fpp-to-xml found at: ${FPP_TO_XML}")
message(STATUS "[fpp-tools] fpp-to-cpp found at: ${FPP_TO_CPP}")
message(STATUS "[fpp-tools] fpp-locate-defs found at: ${FPP_LOCATE_DEFS}")