####
# cmake/target/version.cmake:
#
# A basic versioning target which will produce the version.hpp file.
####
set(FPRIME_VERSION_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/version/generate_version_header.py" CACHE PATH "Script used to generate version.hpp")

function(version_add_global_target TARGET)
    set(OUTPUT_FILE "${CMAKE_BINARY_DIR}/version.hpp")
    # Add check argument when requested
    set(OPTIONAL_CHECK_ARG)
    if (FPRIME_CHECK_FRAMEWORK_VERSION)
        set(OPTIONAL_CHECK_ARG "--check")
    endif()
    add_custom_target("${TARGET}" ALL BYPRODUCTS "${OUTPUT_FILE}"
        COMMAND "${CMAKE_COMMAND}" -E env "PYTHONPATH=${PYTHONPATH}:${FPRIME_FRAMEWORK_PATH}/Autocoders/Python/src"
            "${FPRIME_VERSION_SCRIPT}" "${OUTPUT_FILE}.tmp" "${OPTIONAL_CHECK_ARG}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${OUTPUT_FILE}.tmp" "${OUTPUT_FILE}"
        WORKING_DIRECTORY "${FPRIME_PROJECT_ROOT}"
    )
endfunction()

function(version_add_deployment_target MODULE TARGET SOURCES DEPENDENCIES FULL_DEPENDENCIES)
endfunction()

function(version_add_module_target MODULE_NAME TARGET_NAME SOURCE_FILES DEPENDENCIES)
endfunction(version_add_module_target)
