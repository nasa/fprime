####
# fpp-locs.cmake:
#
# fpp-locs is a special target used to build fpp-locs file output. It is run in a special build of the fprime system
# specifically focused on generating the locator file for fpp.  It registers normal targets for building these items,
# but also comes with a function to run the separate build of fprime.
####

# Properties needed to be forwarded to the internal cmake call
set(NEEDED_PROPERTIES
        FPRIME_CONFIG_DIR
        FPRIME_AC_CONSTANTS_FILE
        FPRIME_ENVIRONMENT_FILE
        FPRIME_SETTINGS_FILE
        FPRIME_LIBRARY_LOCATIONS
        FPRIME_PROJECT_ROOT
        FPRIME_FRAMEWORK_PATH
        CMAKE_TOOLCHAIN_FILE
        CMAKE_BUILD_TYPE
        CMAKE_DEBUG_OUTPUT
        FPRIME_USE_STUBBED_DRIVERS
        FPRIME_USE_BAREMETAL_SCHEDULER
        FPP_TOOLS_PATH
)
set(FPP_CONFIGS
    "${FPRIME_CONFIG_DIR}/AcConstants.fpp"
    "${FPRIME_CONFIG_DIR}/FpConfig.fpp"
    "${FPRIME_FRAMEWORK_PATH}/Fpp/ToCpp.fpp"
)

####
# Run the CMake build for generating the locs.fpp file. Should be called by the primary build in order to build the
# locator files needed by the project.
####
function(generate_fpp_locs)
    set(CALL_PROPS)
    foreach (PROPERTY IN LISTS NEEDED_PROPERTIES)
        if (NOT "${${PROPERTY}}" STREQUAL "")
            list(APPEND CALL_PROPS "-D${PROPERTY}=${${PROPERTY}}")
        endif()
    endforeach()
    # Execute the generate step
    set(LOCS_DIR "${CMAKE_BINARY_DIR}/fpp-locs")
    set(LOCS_FILE "${LOCS_DIR}/locs.fpp")

    # Only generate the FPP locs cache when it doesn't exist.  Otherwise just build.
    if (NOT IS_DIRECTORY LOCS_DIR)
        file(MAKE_DIRECTORY "${LOCS_DIR}")
        execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" "${CMAKE_CURRENT_SOURCE_DIR}" -DFPRIME_FPP_LOCS_BUILD=ON ${CALL_PROPS}
                RESULT_VARIABLE result
                OUTPUT_FILE "${LOCS_DIR}/generate-output.log"
                WORKING_DIRECTORY "${LOCS_DIR}")
        if(result)
            message(FATAL_ERROR "CMake step for generating fpp-locs build failed: ${result}")
        endif()
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} --build . --target fpp-locs
            RESULT_VARIABLE result
            OUTPUT_FILE "${LOCS_DIR}/build-output.log"
            WORKING_DIRECTORY "${LOCS_DIR}" )
    if(result)
        message(FATAL_ERROR "CMake step for building fpp-locs build failed: ${result}")
    endif()
    set(FPP_LOCS_FILE "${LOCS_FILE}" CACHE INTERNAL "File containing the locators for FPP" FORCE)
endfunction()

####
# fpp-locs function `add_deployment_target`:
#
# Does nothing.  Fpp locations are truly global.
####
function(add_deployment_target MODULE TARGET SOURCES DEPENDENCIES FULL_DEPENDENCIES)
    if (NOT TARGET MODULE)
        set(EMPTY_C_SRC "${CMAKE_CURRENT_BINARY_DIR}/empty.c")
        file(WRITE "${EMPTY_C_SRC}" "#define CMAKE_EMPTY_SOURCE\n")
        add_library(${MODULE} "${EMPTY_C_SRC}") # Fake target to appease those who hand-edit targets outside of fprime
    endif()
endfunction()

####
# fpp-locs function `add_global_target`:
#
# This function takes the INPUTS property added to our global
####
function(add_global_target TARGET_NAME)
    find_program(FPP_LOCATE_DEFS fpp-locate-defs)
    if (DEFINED FPP_LOCATE_DEFS-NOTFOUND)
        message(FATAL_ERROR "fpp tools not found, please install them onto your system path")
    endif()
    add_custom_target("${TARGET_NAME}" COMMAND "${FPP_LOCATE_DEFS}" -d "${CMAKE_BINARY_DIR}" ${FPP_CONFIGS} $<TARGET_PROPERTY:${TARGET_NAME},INPUTS> > "${CMAKE_BINARY_DIR}/locs.fpp" VERBATIM)
    set_target_properties("${TARGET_NAME}" PROPERTIES INPUTS "")
endfunction(add_global_target)

####
# fpp-locs function `add_module_target`:
#
# This target appends sources that are handled by the fpp autocoder to the source list of the fpp-locs-gen target. This
# target will run the location generation from those matching sources
#
# - **MODULE:** name of the module
# - **TARGET:** name of the top-target (e.g. dict). Use ${MODULE_NAME}_${TARGET_NAME} for a module specific target
# - **SOURCE_FILES:** list of source file inputs from the CMakeList.txt setup
# - **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
####
function(add_module_target MODULE TARGET SOURCES DEPENDENCIES)
    include(autocoder/default)
    include(autocoder/fpp)
    foreach(SOURCE IN LISTS SOURCES)
        is_supported("${SOURCE}")
        if (IS_SUPPORTED)
            get_target_property(TARGET_INPUTS "${TARGET}" INPUTS)
            set(TARGET_INPUTS " ${TARGET_INPUTS} ${SOURCE}") #Space separated list, to work around generator failure
            set_target_properties("${TARGET}" PROPERTIES INPUTS "${TARGET_INPUTS}")
        endif()
    endforeach()
    # Since fpp-locs runs as an independent build, we must force a target to exist
    if (NOT TARGET MODULE)
        set(EMPTY_C_SRC "${CMAKE_CURRENT_BINARY_DIR}/empty.c")
        file(WRITE "${EMPTY_C_SRC}" "#define CMAKE_EMPTY_SOURCE\n")
        add_library(${MODULE} "${EMPTY_C_SRC}") # Fake target to appease those who hand-edit targets outside of fprime
    endif()
endfunction(add_module_target)
