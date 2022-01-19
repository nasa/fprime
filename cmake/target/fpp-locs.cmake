####
# fpp-locs.cmake:
#
# fpp-locs is a special target used to build fpp-locs file output. It is run in a special build of the fprime system
# specifically focused on generating the locator file for fpp.  It registers normal targets for building these items,
# but also comes with a function to run the separate build of fprime.
####

# Static and configuration FPP files
set(FPP_CONFIGS
    "${FPRIME_CONFIG_DIR}/AcConstants.fpp"
    "${FPRIME_CONFIG_DIR}/FpConfig.fpp"
    "${FPRIME_FRAMEWORK_PATH}/Fpp/ToCpp.fpp"
)
set(FPP_LOCATE_DEFS_HELPER "${FPRIME_FRAMEWORK_PATH}/cmake/autocoder/fpp-wrapper/fpp-redirect-helper")
set(FPP_DEPEND_PARALLELIZE "${FPRIME_FRAMEWORK_PATH}/cmake/autocoder/fpp-wrapper/fpp-depend-parallelize.py")
set(FPP_LOCS_FILE "${CMAKE_BINARY_DIR}/locs.fpp")

####
# Function `determine_global_fpps`:
#
# Processes the global set of modules and determines all FPP files.
# OUTPUT_VAR: variable storing the output of this call
# MODULES: global set of modules passed in
####
function(determine_global_fpp_inputs MODULES)
    include(autocoder/fpp)
    # Loop through the modules
    foreach (MODULE IN LISTS MODULES)
        get_target_property(SOURCES "${MODULE}" FP_SRC)
        # Check each source for FPP support
        foreach(SOURCE IN LISTS SOURCES)
            is_supported("${SOURCE}")
            if (IS_SUPPORTED)
                set_property(GLOBAL APPEND PROPERTY ALL_FPPS "${SOURCE}")
                set_property(TARGET "${MODULE}" APPEND PROPERTY FPP_INPUTS "${SOURCE}")
            endif()
        endforeach()
    endforeach()
endfunction(determine_global_fpp_inputs)

####
# Function `generate_locations`:
#
# Generates the FPP locations index. This is needed for all subsequent steps of FPP.
####
function(generate_locations)
    if (NOT FPP_LOCATE_DEFS)
        message(FATAL_ERROR "Unable to determine fpp-locate-defs executable")
    endif()
    message(STATUS "Generating FPP location index")
    get_property(FPP_FILES GLOBAL PROPERTY ALL_FPPS)
    execute_process(COMMAND "${FPP_LOCATE_DEFS_HELPER}" "${FPP_LOCS_FILE}" "${FPP_LOCATE_DEFS}" -d "${CMAKE_BINARY_DIR}" ${FPP_CONFIGS} ${FPP_FILES} COMMAND_ERROR_IS_FATAL ANY)
    message(STATUS "Generating FPP location index - DONE")
endfunction()

####
# Function `generate_dependencies`:
#
# Generate dependencies for FPP modules. This is done here for performance, and the generated caches will be read later.
# MODULES: modules to generate from
####
function(generate_dependencies MODULES)
    if (NOT FPP_DEPEND)
        message(FATAL_ERROR "Unable to determine fpp-depend executable")
    endif()
    if (NOT PYTHON)
        message(FATAL_ERROR "Unable to determine python executable")
    endif()
    message(STATUS "Generating FPP dependency caches")
    set(CACHE_FILE "${CMAKE_BINARY_DIR}/fpp-depend-input" )
    file(WRITE "${CACHE_FILE}" "")
    foreach(MODULE IN LISTS MODULES)
        get_target_property(TARGET_FPPS "${MODULE}" FPP_INPUTS)
        get_target_property(BIN_DIR "${MODULE}" FP_BIND)
        if (TARGET_FPPS)
            file(APPEND "${CACHE_FILE}" "${BIN_DIR};${TARGET_FPPS}\n")
        endif()
    endforeach()
    execute_process(COMMAND "${PYTHON}" "${FPP_DEPEND_PARALLELIZE}" "${FPP_DEPEND}" "${FPP_LOCS_FILE}" "${CACHE_FILE}" COMMAND_ERROR_IS_FATAL ANY)
    message(STATUS "Generating FPP dependency caches - DONE")
endfunction(generate_dependencies)

####
# Function `add_global_target`:
#
# Performs special FPP setup and handling.
####
function(add_global_target TARGET_NAME)
    get_property(GLOBAL_MODULES GLOBAL PROPERTY FPRIME_MODULES)
    # One-time FPP setup done to absolve performance issues
    determine_global_fpp_inputs("${GLOBAL_MODULES}")
    generate_locations()
    generate_dependencies("${GLOBAL_MODULES}")
endfunction(add_global_target)

####
# Not defined to prevent defaults from engaging
####
function(add_deployment_target MODULE TARGET SOURCES DEPENDENCIES FULL_DEPENDENCIES)
endfunction()

####
# Not defined to prevent defaults from engaging
####
function(add_module_target MODULE TARGET SOURCES DEPENDENCIES)
endfunction(add_module_target)