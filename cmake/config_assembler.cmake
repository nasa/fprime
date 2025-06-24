####
# config_assembler.cmake:
#
# CMake configuration handling function.
####
include_guard()
# Create a target to act as an interface to all fprime configuration modules
set(FPRIME__INTERNAL_CONFIG_TARGET_NAME "__fprime_config")
add_library(${FPRIME__INTERNAL_CONFIG_TARGET_NAME} INTERFACE)

####
# Function `fprime__internal_process_configuration_sources`:
#
# This function will process the configuration sources from various calls to set up configuration modules. It will
# ensure that SOURCES/HEADERS are unique across module and will ensure CONFIGURATION_OVERRIDES override existing source
# and header files.
#
# Arguments:
# - `MODULE_NAME`: the name of the module being processed
# - `SOURCES`: list of sources to process
# - `AUTOCODER_INPUTS`: list of autocoder inputs to process
# - `HEADERS`: list of headers to process
# - `OVERRIDES`: list of configuration overrides to process
# - `DEPENDS`: list of dependencies to append to
#
# Returns:
# - `INTERNAL_SOURCES`: list of sources in their final configuration location (set in caller)
# - `INTERNAL_AUTOCODER_INPUTS`: list of autocoder inputs in their final configuration location (set in caller)
# - `INTERNAL_HEADERS`: list of headers in their final configuration location (set in caller)
# - `INTERNAL_DEPENDS`: list of dependencies, new and old
####
function(fprime__internal_process_configuration_sources MODULE_NAME SOURCES AUTOCODER_INPUTS HEADERS OVERRIDES DEPENDS)
    # Process source files and update INTERNAL_SOURCES in caller and track new dependencies
    fprime__internal_process_configuration_source_set(
        "${MODULE_NAME}" "${SOURCES}" FALSE
    )
    set(INTERNAL_SOURCES "${PROCESSED_SOURCES}" PARENT_SCOPE)
    set(DEPENDS ${DEPENDS} ${NEW_DEPENDS})
    # Process source files and update INTERNAL_AUTOCODER_INPUTS in caller and track new dependencies
    fprime__internal_process_configuration_source_set(
        "${MODULE_NAME}" "${AUTOCODER_INPUTS}" FALSE
    )
    set(INTERNAL_AUTOCODER_INPUTS "${PROCESSED_SOURCES}" PARENT_SCOPE)
    set(DEPENDS ${DEPENDS} ${NEW_DEPENDS})
    # Process header files and update INTERNAL_HEADERS in caller and track new dependencies
    fprime__internal_process_configuration_source_set(
        "${MODULE_NAME}" "${HEADERS}" FALSE
    )
    set(INTERNAL_HEADERS "${PROCESSED_SOURCES}" PARENT_SCOPE)
    set(DEPENDS ${DEPENDS} ${NEW_DEPENDS})
    # Process configuration overrides. Since these are already in a module, they need not be updated in caller.
    # New dependencies are tracked.
    fprime__internal_process_configuration_source_set(
        "${MODULE_NAME}" "${OVERRIDES}" TRUE
    )
    set(INTERNAL_DEPENDS ${DEPENDS} ${NEW_DEPENDS} PARENT_SCOPE)
endfunction()

####
# Function `fprime__internal_process_configuration_source_set`:
#
# Processes a single set of configuration files checking to see if files collide and if they must collide.
#
# Arguments:
# - `MODULE_NAME`: the name of the module being processed
# - `SOURCE_SET`: list of sources to process
# - `EXPECT_OVERRIDE`: if true, the source must exist and will be overridden, false if it must not exist
#
# Returns:
# - `PROCESSED_SOURCES`: list (set in caller)
# - `NEW_DEPENDS`: list of new dependencies (set in caller)
####
function(fprime__internal_process_configuration_source_set MODULE_NAME SOURCE_SET EXPECT_OVERRIDE)
    list(REMOVE_DUPLICATES SOURCE_SET)
    set(RETURNED_SOURCES)
    set(NEW_DEPENDS)

    foreach(SOURCE IN LISTS SOURCE_SET)
        get_filename_component(SOURCE_NAME "${SOURCE}" NAME)

        fprime_internal_get_configuration_destination("${MODULE_NAME}" "${SOURCE_NAME}")

        # Check if the source cannot exist, and yet it was found
        if (NOT EXPECT_OVERRIDE AND DESTINATION_OVERRIDE)
            message(FATAL_ERROR
                "${SOURCE_NAME} is SOURCE/HEADER but overrides existing file: ${DESTINATION}. Use CONFIGURATION_OVERRIDES.")
        # Check if the source must exist, and yet it was not found
        elseif (EXPECT_OVERRIDE AND NOT DESTINATION_OVERRIDE)
            message(FATAL_ERROR
                "${SOURCE_NAME} is CONFIGURATION_OVERRIDE but overrides nonexistent file: ${DESTINATION}. Use SOURCES/HEADERS.")
        # If the source must exist and it was found, overwrite it
        elseif(EXPECT_OVERRIDE)
            fprime_cmake_debug_message("[config] Overriding ${DESTINATION} with ${SOURCE}")
            file(COPY_FILE "${SOURCE}" "${DESTINATION}" ONLY_IF_DIFFERENT)
            list(APPEND NEW_DEPENDS "${DESTINATION_MODULE}")
            set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${SOURCE}")
        # If the source is new, move it to the binary directory
        else()
            fprime_cmake_debug_message("[config] Initial config ${DESTINATION} from ${SOURCE}")
            list(APPEND RETURNED_SOURCES "${DESTINATION}")
            file(MAKE_DIRECTORY "${DESTINATION_DIRECTORY}")
            file(COPY_FILE "${SOURCE}" "${DESTINATION}" ONLY_IF_DIFFERENT)
            set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${SOURCE}")
        endif()
    endforeach()
    set(PROCESSED_SOURCES "${RETURNED_SOURCES}" PARENT_SCOPE)
    set(NEW_DEPENDS "${NEW_DEPENDS}" PARENT_SCOPE)
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
function(fprime_internal_get_configuration_destination MODULE_NAME NEW_CONFIG_NAME)
    # Get all registered configuration modules
    get_property(CONFIG_MODULES GLOBAL PROPERTY FPRIME_CONFIG_MODULES)
    foreach(CONFIG_MODULE IN LISTS CONFIG_MODULES)
        # Read the sources, headers, and autocoder inputs from the module
        get_target_property(CONFIG_SOURCES ${CONFIG_MODULE} SUPPLIED_SOURCES)
        get_target_property(CONFIG_HEADERS ${CONFIG_MODULE} SUPPLIED_HEADERS)
        get_target_property(CONFIG_AUTOCODER_INPUTS ${CONFIG_MODULE} SUPPLIED_AUTOCODER_INPUTS)

        # Loop through all read files
        foreach(CONFIG_FILE IN LISTS CONFIG_SOURCES CONFIG_HEADERS CONFIG_AUTOCODER_INPUTS)
            # Determine if the names match, if so set the destination
            get_filename_component(CONFIG_NAME "${CONFIG_FILE}" NAME)
            if (NEW_CONFIG_NAME STREQUAL CONFIG_NAME)
                set(DESTINATION "${CONFIG_FILE}" PARENT_SCOPE)
                set(DESTINATION_MODULE "${CONFIG_MODULE}" PARENT_SCOPE)
                set(DESTINATION_OVERRIDE TRUE PARENT_SCOPE)
                return()
            endif()
        endforeach()
    endforeach()
    # F Prime sub-builds still need to calculate (and copy) the files to the base build cache specified by FPRIME_BINARY_DIR
    # This is needed for locations generation to calculate the correct paths.
    #
    # This code calculates the relative path from the cmake build cache of the current built to the current binary directory.
    # This is the relative path within the build current build cache.  Then it applies this relative path to FPRIME_BINARY_DIR
    # if it is set, otherwise it just recalculates the current binary directory.
    cmake_path(RELATIVE_PATH CMAKE_CURRENT_BINARY_DIR BASE_DIRECTORY ${CMAKE_BINARY_DIR} OUTPUT_VARIABLE RELATIVE_PATH)
    
    set(DESTINATION_BASE "${CMAKE_BINARY_DIR}")
    if (DEFINED FPRIME_BINARY_DIR)
        set(DESTINATION_BASE "${FPRIME_BINARY_DIR}")
    endif()
    get_filename_component(SOURCE_NAME "${NEW_CONFIG_NAME}" NAME)
    set(DESTINATION_DIRECTORY "${DESTINATION_BASE}/${RELATIVE_PATH}")
    set(DESTINATION "${DESTINATION_DIRECTORY}/${SOURCE_NAME}" PARENT_SCOPE)
    set(DESTINATION_MODULE "${MODULE_NAME}" PARENT_SCOPE)
    set(DESTINATION_DIRECTORY "${DESTINATION_DIRECTORY}" PARENT_SCOPE)
    set(DESTINATION_OVERRIDE FALSE PARENT_SCOPE)
endfunction()

