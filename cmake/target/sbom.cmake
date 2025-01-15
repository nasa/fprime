####
# cmake/target/sbom.cmake:
#
# A target used to add SBOM generation to the build. Will be invoked when running the "all" target
# and installed into the build_artifacts directory underneath the platform folder.
####
set(REDIRECTOR "${CMAKE_CURRENT_LIST_DIR}/tools/redirector.py")

####
# sbom_add_global_target:
# 
# Used to register a global target that will build with "all" and generates the SBOM.
#
#####
function(sbom_add_global_target TARGET)
    find_program(SYFT NAMES syft)
    # Check if syft is available before running
    if (SYFT)
        add_custom_target("${TARGET}" ALL
            COMMAND
            # Redirect to cleanly capture standard out
            ${PYTHON} ${REDIRECTOR} "${CMAKE_BINARY_DIR}/${PROJECT_NAME}_sbom.json"
            # syft arguments
            "${SYFT}" "dir:${FPRIME_PROJECT_ROOT}" -o spdx-json
            # Excludes .github paths not in the root of the project as those should not be activated by the project
            --exclude '*/**/.github'
            DEPENDS $<TARGET_PROPERTY:${TARGET},SBOM_DEPENDENCIES>
        )
        # Install the SBOM file
        install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}_sbom.json" DESTINATION ${TOOLCHAIN_NAME} COMPONENT ${TARGET})
        add_custom_command(TARGET "${TARGET}" POST_BUILD COMMAND "${CMAKE_COMMAND}"
                          -DCMAKE_INSTALL_COMPONENT=${TARGET} -P ${CMAKE_BINARY_DIR}/cmake_install.cmake)
    else()
        message(STATUS "[INFO] to find 'syft' on PATH, please install to generate software bill-of-materials")
    endif()
endfunction()

# For deployments
function(sbom_add_deployment_target MODULE TARGET SOURCES DEPENDENCIES FULL_DEPENDENCIES)
    if (TARGET "${TARGET}")
        append_list_property("${MODULE}" TARGET "${TARGET}" PROPERTY SBOM_DEPENDENCIES)
    endif()
endfunction()

# Used to register all modules
function(sbom_add_module_target MODULE TARGET SOURCE_FILES DEPENDENCIES)
    if (TARGET "${TARGET}")
        append_list_property("${MODULE}" TARGET "${TARGET}" PROPERTY SBOM_DEPENDENCIES)
    endif()
endfunction()
