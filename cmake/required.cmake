####
# required.cmake:
#
# Required tools for the fprime CMake system. This system pre-checks all the tools needed to generate and build through
# cmake. These items are strictly required and will fail if not found, which will prevent confusing errors further
# within the build process.
####
include(autocoder/fpp)

find_program(FPUTIL NAMES fprime-util)
find_program(PYTHON NAMES python3 python)

locate_fpp_tools()

# Check python was found
if (NOT FPUTIL)
    message(FATAL_ERROR "fprime-util was not found. Please install with 'pip install fprime-tools'")
elseif (NOT PYTHON)
    message(FATAL_ERROR "python3 was not found. Please ensure python3 is on the path.")
elseif(NOT FPP_FOUND)
    message(FATAL_ERROR "fpp tools were not found. Please install the fpp tool suite.")
endif()



message(STATUS "[python3] python3 found at: ${PYTHON}")
message(STATUS "[fpp-tools] fpp-depend found at: ${FPP_DEPEND}")
message(STATUS "[fpp-tools] fpp-to-xml found at: ${FPP_TO_XML}")
message(STATUS "[fpp-tools] fpp-to-cpp found at: ${FPP_TO_CPP}")
message(STATUS "[fpp-tools] fpp-locate-defs found at: ${FPP_LOCATE_DEFS}")
