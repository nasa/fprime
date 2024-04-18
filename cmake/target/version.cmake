####
# cmake/target/version.cmake:
#
# A basic versioning target which will produce the version files.
####
set(FPRIME_VERSION_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/version/generate_version_header.py" CACHE PATH "Script used to generate version.hpp")

function(version_add_global_target TARGET)
    set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/versions")
    set(OUTPUT_HPP "${OUTPUT_DIR}/version.hpp")
    set(OUTPUT_FILES "${OUTPUT_HPP}" "${OUTPUT_DIR}/version.project" "${OUTPUT_DIR}/version.framework" "${OUTPUT_DIR}/version.libraries")
    file(MAKE_DIRECTORY ${OUTPUT_DIR})
    # Add check argument when requested
    set(OPTIONAL_CHECK_ARG)
    string(REGEX REPLACE ";" ":"  FPRIME_LIBRARY_LOCATIONS_CSV "${FPRIME_LIBRARY_LOCATIONS}")
    if (FPRIME_CHECK_FRAMEWORK_VERSION)
        set(OPTIONAL_CHECK_ARG "--check")
    endif()
    add_custom_target("${TARGET}" ALL BYPRODUCTS "${OUTPUT_FILE}"
        COMMAND "${CMAKE_COMMAND}" 
            -E env "PYTHONPATH=${PYTHONPATH}:${FPRIME_FRAMEWORK_PATH}/Autocoders/Python/src" 
                    "FPRIME_PROJECT_ROOT=${FPRIME_PROJECT_ROOT}"
                    "FPRIME_FRAMEWORK_PATH=${FPRIME_FRAMEWORK_PATH}"
                    "FPRIME_LIBRARY_LOCATIONS=${FPRIME_LIBRARY_LOCATIONS_CSV}"
            "${FPRIME_VERSION_SCRIPT}" "${OUTPUT_DIR}" "${OPTIONAL_CHECK_ARG}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${OUTPUT_HPP}.tmp" "${OUTPUT_FILES}"
        WORKING_DIRECTORY "${FPRIME_PROJECT_ROOT}"
    )
endfunction()

function(version_add_deployment_target MODULE TARGET SOURCES DEPENDENCIES FULL_DEPENDENCIES)
endfunction()

function(version_add_module_target MODULE_NAME TARGET_NAME SOURCE_FILES DEPENDENCIES)
endfunction(version_add_module_target)
