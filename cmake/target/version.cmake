
####
# cmake/target/version.cmake:
#
# A basic versioning target which will produce the version.hpp file.
####
set(FPRIME_VERSION_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/version/generate_version_header.py" CACHE PATH "Script used to generate version.hpp")

function(add_global_target TARGET)
    set(OUTPUT_FILE "${CMAKE_BINARY_DIR}/version.hpp")
    add_custom_command(
       OUTPUT "${OUTPUT_FILE}" __PHONY__
       COMMAND ${CMAKE_COMMAND} -E chdir "${FPRIME_PROJECT_ROOT}" "${FPRIME_VERSION_SCRIPT}" "${OUTPUT_FILE}")
    add_custom_target(${TARGET} DEPENDS "${OUTPUT_FILE}" __PHONY__)
endfunction()

function(add_deployment_target MODULE TARGET SOURCES DEPENDENCIES FULL_DEPENDENCIES)
endfunction()

function(add_module_target MODULE_NAME TARGET_NAME SOURCE_FILES DEPENDENCIES)
endfunction(add_module_target)
