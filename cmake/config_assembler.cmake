####
# Function `fprime__internal_process_configuration_sources`:
#
# This function will process the configuration sources from various calls to set up configuration modules. It will
# ensure that SOURCES/HEADERS are unique across module and will ensure CONFIGURATION_OVERRIDES override existing source
# and header files.
#
# Arguments:
# - `SOURCES`: list of sources to process
# - `AUTOCODER_INPUTS`: list of autocoder inputs to process
# - `HEADERS`: list of headers to process
# - `OVERRIDES`: list of configuration overrides to process
#
# Returns:
# - `INTERNAL_SOURCES`: list of sources in their final configuration location (set in caller)
# - `INTERNAL_HEADERS`: list of headers in their final configuration location (set in caller)
####
function(fprime__internal_process_configuration_sources SOURCES AUTOCODER_INPUTS HEADERS OVERRIDES)
    # Process source files and update INTERNAL_SOURCES in caller
    fprime__internal_process_configuration_source_set(
        "${CONFIG_DIRECTORY}" "${SOURCES}" FALSE
    )
    set(INTERNAL_SOURCES "${PROCESSED_SOURCES}" PARENT_SCOPE)
    # Process source files and update INTERNAL_AUTOCODER_INPUTS in caller
    fprime__internal_process_configuration_source_set(
        "${CONFIG_DIRECTORY}" "${AUTOCODER_INPUTS}" FALSE
    )
    set(INTERNAL_AUTOCODER_INPUTS "${PROCESSED_SOURCES}" PARENT_SCOPE)
    # Process header files and update INTERNAL_HEADERS in caller
    fprime__internal_process_configuration_source_set(
        "${CONFIG_DIRECTORY}" "${HEADERS}" FALSE
    )
    set(INTERNAL_HEADERS "${PROCESSED_SOURCES}" PARENT_SCOPE)
    # Process configuration overrides. Since these are already in a module, they need not be updated in caller.
    fprime__internal_process_configuration_source_set(
        "${CONFIG_DIRECTORY}" "${CONFIGURATION_OVERRIDES}" TRUE
    )
endfunction()

####
# Function `fprime__internal_process_configuration_source_set`:
#
# Processes a single set of configuration files checking to see if files collide and if they must collide.
#
# Arguments:
# - `SOURCE_SET`: list of sources to process
# - `MUST_EXIST`: if true, the source must exist in the configuration directory, false if it must not exist
#
# Returns:
# - `PROCESSED_SOURCES`: list (set in caller)
####
function(fprime__internal_process_configuration_source_set CONFIG_DIR SOURCE_SET MUST_EXIST)
    list(REMOVE_DUPLICATES SOURCE_SET)
    set(RETURNED_SOURCES)
    foreach(SOURCE IN LISTS SOURCE_SET)
        get_filename_component(SOURCE_NAME "${SOURCE}" NAME)

        fprime_internal_get_configuration_destination("${SOURCE_NAME}")

        # Check if the source cannot exist, and yet it was found
        if (NOT MUST_EXIST AND DESTINATION)
            message(FATAL_ERROR
                "${CONFIG_RELATIVE} is SOURCE/HEADER but overrides existing file. Use CONFIGURATION_OVERRIDES.")
        # Check if the source must exist, and yet it was not found
        elseif (MUST_EXIST AND NOT DESTINATION)
            message(FATAL_ERROR
                "${CONFIG_RELATIVE} is CONFIGURATION_OVERRIDE but overrides non-existent file. Use SOURCES/HEADERS.")
        # If the source must exist and it was found, overwrite it
        elseif(MUST_EXIST)
            file(COPY "${SOURCE}" DESTINATION "${DESTINATION}")
        # If the source is new, move it to the binary directory
        else()
            list(APPEND RETURNED_SOURCES "${SOURCE}")
            file(COPY "${SOURCE}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
        endif()
    endforeach()
    set(PROCESSED_SOURCES "${RETURNED_SOURCES}" PARENT_SCOPE)
endfunction()

####
# Function `fprime_internal_get_configuration_destination`:
#
# This function will determine the destination of a configuration file by checking to see if the file is in use by any
# other configuration modules. If it is, it will return the destination of the read from that module's original source
# via the DESTINATION variable. If it is not, it will unset the DESTINATION variable in PARENT_SCOPE.
#
# Arguments:
# - `CONFIG_NAME`: the relative path to the configuration file
#
# Returns:
# - `DESTINATION`: the destination of the configuration file or unset (in caller)
####
function(fprime_internal_get_configuration_destination NEW_CONFIG_NAME)
    # Get all registered configuration modules
    get_property(CONFIG_MODULES GLOBAL PROPERTY FPRIME_CONFIG_MODULES)
    foreach(CONFIG_MODULE IN LISTS CONFIG_MODULES)
        # Read the sources, headers, and autocoder inputs from the module
        get_target_property(CONFIG_SOURCES ${CONFIG_MODULE} SOURCES)
        get_target_property(CONFIG_HEADERS ${CONFIG_MODULE} HEADERS)
        get_target_property(CONFIG_AUTOCODER_INPUTS ${CONFIG_MODULE} AUTOCODER_INPUTS)

        # Loop through all read files
        foreach(CONFIG_FILE IN LISTS CONFIG_SOURCES CONFIG_HEADERS CONFIG_AUTOCODER_INPUTS)
            # Determine if the names match, if so set the destination
            get_filename_component(CONFIG_NAME "${CONFIG_FILE}" NAME)
            if (NEW_CONFIG_NAME STREQUAL CONFIG_NAME)
                set(DESTINATION "${CONFIG_FILE}" PARENT_SCOPE)
                return()
            endif()
        endforeach()
    endforeach()
    # If no match was found, unset the destination
    unset(DESTINATION PARENT_SCOPE)
endfunction()

