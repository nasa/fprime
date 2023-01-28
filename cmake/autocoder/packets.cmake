####
# autocoder/packets.cmake
#
# Packets autocoder.
#####
include(utilities)
include(autocoder/helpers)
include(autocoder/ai-shared)

set(PACKETS_AUTOCODER_SCRIPT "${FPRIME_FRAMEWORK_PATH}/Autocoders/Python/bin/tlm_packet_gen.py")

autocoder_setup_for_individual_sources()
####
# `packets_is_supported`:
#
# Required function, processes Ai.xml files.
# `AC_INPUT_FILE` potential input to the autocoder
# ...: any number of arguments representing a list of previously generated files
####
function(packets_is_supported AC_INPUT_FILE)
    autocoder_support_by_suffix("Packets.xml" "${AC_INPUT_FILE}" TRUE)
endfunction (packets_is_supported)

####
# `determine_topology_files`:
#
# Get topology determined from packet file.
####
function(determine_topology_files AC_INPUT_FILE)
    file(READ "${AC_INPUT_FILE}" FILE_CONTENTS)
    string(REGEX REPLACE ".*<import_topology>([^>]*)</import_topology>.*" "\\1" TOPOLOGY_FILE "${FILE_CONTENTS}")
    # This will work as long as the topology ai file is not part of the fprime core "library" code
    set(FULL_TOPOLOGY_FILE "${CMAKE_BINARY_DIR}/${TOPOLOGY_FILE}" PARENT_SCOPE)
endfunction(determine_topology_files)

####
# `packets_setup_autocode`:
#
# Required function, sets up a custom command to produce Ac.hpp and Ac.cpp files.
####
function(packets_setup_autocode AC_INPUT_FILE)
    determine_topology_files("${AC_INPUT_FILE}")
    get_filename_component(AC_INPUT_FILE_NO_PATH "${AC_INPUT_FILE}" NAME)

    string(REPLACE ";" ":" FPRIME_BUILD_LOCATIONS_SEP "${FPRIME_BUILD_LOCATIONS}")
    string(REPLACE "Packets.xml" "PacketsAc.cpp" CPP_FILE "${AC_INPUT_FILE_NO_PATH}")
    string(REPLACE "Packets.xml" "PacketsAc.hpp" HPP_FILE "${AC_INPUT_FILE_NO_PATH}")

    set(GENERATED_FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${CPP_FILE}"
        "${CMAKE_CURRENT_BINARY_DIR}/${HPP_FILE}"
    )
    add_custom_command(
        OUTPUT ${GENERATED_FILES}
        COMMAND
            PYTHONPATH=${PYTHON_AUTOCODER_DIR}/src:${PYTHON_AUTOCODER_DIR}/utils
            BUILD_ROOT=${FPRIME_BUILD_LOCATIONS_SEP}:${CMAKE_BINARY_DIR}:${CMAKE_BINARY_DIR}/F-Prime
            "${PYTHON}" "${PACKETS_AUTOCODER_SCRIPT}" "${AC_INPUT_FILE}"
        DEPENDS "${AC_INPUT_FILE}" "${FULL_TOPOLOGY_FILE}"
    )
    set(AUTOCODER_GENERATED "${GENERATED_FILES}" PARENT_SCOPE)
    set(AUTOCODER_DEPENDENCIES "" PARENT_SCOPE)
endfunction(packets_setup_autocode)
