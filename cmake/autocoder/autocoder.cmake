####
# autocoder/autocoder.cmake:
#
# Autocoder setup and support file. This performs all of the general autocoder functions, running the specific functions
# defined within the individual autocoders. This gives the ability to run a set of autocoders to produce files.
#
# Note: autocoders need to be run by targets. See target/target.cmake.
####
include_guard()
include(utilities)
include(autocoder/helpers)

# Allowed return values
set(FPRIME_AUTOCODER_UNSUPPORTED AUTOCODER_GENERATED AUTOCODER_SCRIPT AUTOCODER_INPUTS AUTOCODER_INCLUDES)
set(FPRIME_AUTOCODER_REQUIRED AUTOCODER_GENERATED_AUTOCODER_INPUTS AUTOCODER_GENERATED_BUILD_SOURCES AUTOCODER_GENERATED_OTHER)
set(FPRIME_AUTOCODER_OPTIONAL AUTOCODER_DEPENDENCIES)

####
# run_ac_set:
#
# Run a set of autocoder allowing back-to-back execution of a set of autocoders. SOURCES are the source files that are
# input into the autocoder filters.  Extra arguments may be an include path for an autocoder (e.g. autocoder/fpp).
#
# BUILD_TARGET_NAME: name of the build target getting attached autocoding
# SOURCES: source file input list
# ...: autocoder include
####
function (run_ac_set BUILD_TARGET_NAME)
    # Get all sources available for autocode processing
    get_target_property(AUTOCODER_INPUT_SOURCES "${BUILD_TARGET_NAME}" AUTOCODER_INPUTS)
    # Get the source list, if passed in
    set(AC_LIST)
    if (ARGN)
        set(AC_LIST "${ARGN}")
    endif()
    # Do not init GENERATED_FILE_LIST as it is read from previous AC runs above
    
    # Create a hash of the autocoder set to isolate results
    string(SHA1 "AC_SET_HASH" "${AC_LIST}")
    
    foreach(AC_CMAKE IN LISTS AC_LIST)
        run_ac("${BUILD_TARGET_NAME}" "${AC_CMAKE}" "${AUTOCODER_INPUT_SOURCES}" "${GENERATED_FILE_LIST}" "${AC_SET_HASH}")
        get_property(AUTOCODER_GENERATED_AUTOCODER_INPUTS_VALUES TARGET "${BUILD_TARGET_NAME}" PROPERTY "${AC_SET_HASH}_AUTOCODER_GENERATED_AUTOCODER_INPUTS")
        list(APPEND AUTOCODER_INPUT_SOURCES ${AUTOCODER_GENERATED_AUTOCODER_INPUTS_VALUES})
    endforeach()
    

    # Read from hash-specific properties for this autocoder set
    get_property(AUTOCODER_GENERATED_VALUES TARGET "${BUILD_TARGET_NAME}" PROPERTY "${AC_SET_HASH}_AUTOCODER_GENERATED")
    get_property(AUTOCODER_GENERATED_BUILD_SOURCES_VALUES TARGET "${BUILD_TARGET_NAME}" PROPERTY "${AC_SET_HASH}_AUTOCODER_GENERATED_BUILD_SOURCES")
    get_property(AUTOCODER_GENERATED_AUTOCODER_INPUTS_VALUES TARGET "${BUILD_TARGET_NAME}" PROPERTY "${AC_SET_HASH}_AUTOCODER_GENERATED_AUTOCODER_INPUTS")
    get_property(AUTOCODER_DEPENDENCIES_VALUES TARGET "${BUILD_TARGET_NAME}" PROPERTY "${AC_SET_HASH}_AUTOCODER_DEPENDENCIES")
    get_property(AUTOCODER_GENERATED_OTHER_VALUES TARGET "${BUILD_TARGET_NAME}" PROPERTY "${AC_SET_HASH}_AUTOCODER_GENERATED_OTHER")
    
    # Append to final target properties (aggregate of all runs)
    append_list_property("${AUTOCODER_GENERATED_VALUES}" TARGET "${BUILD_TARGET_NAME}" PROPERTY AC_GENERATED)
    append_list_property("${AUTOCODER_GENERATED_AUTOCODER_INPUTS_VALUES}" TARGET "${BUILD_TARGET_NAME}" PROPERTY AUTOCODER_INPUTS)
    # Cannot use `target_sources` as it does not respect the "GENERATED" flag. Thus the sources need to be added
    # to the SOURCES property directly.
    append_list_property("${AUTOCODER_GENERATED_BUILD_SOURCES_VALUES}" TARGET "${BUILD_TARGET_NAME}" PROPERTY SOURCES)
    append_list_property("${AUTOCODER_DEPENDENCIES_VALUES}" TARGET "${BUILD_TARGET_NAME}" PROPERTY LINK_LIBRARIES)
    append_list_property("${AUTOCODER_DEPENDENCIES_VALUES}" TARGET "${BUILD_TARGET_NAME}" PROPERTY INTERFACE_LINK_LIBRARIES)
    append_list_property("${AUTOCODER_DEPENDENCIES_VALUES}" TARGET "${BUILD_TARGET_NAME}" PROPERTY FPRIME_DEPENDENCIES)
    # Invalidate the TRANSITIVE_DEPENDENCIES on the target
    if (AUTOCODER_DEPENDENCIES_VALUES)
        set_property(TARGET "${BUILD_TARGET_NAME}" PROPERTY TRANSITIVE_DEPENDENCIES)
    endif()
    # CMake claims that all generated files are marked generated. This asserts this fact.
    get_target_property(ALL_GENERATED "${BUILD_TARGET_NAME}" AC_GENERATED)
    foreach(SOURCE IN LISTS ALL_GENERATED)
        get_source_file_property(IS_GENERATED ${SOURCE} GENERATED)
        fprime_cmake_ASSERT("${SOURCE} is not marked generated." IS_GENERATED)
    endforeach()
    
    # Set variables in parent scope for this run's results
    set(AUTOCODER_GENERATED "${AUTOCODER_GENERATED_VALUES}" PARENT_SCOPE)
    set(AUTOCODER_GENERATED_BUILD_SOURCES "${AUTOCODER_GENERATED_BUILD_SOURCES_VALUES}" PARENT_SCOPE)
    set(AUTOCODER_GENERATED_AUTOCODER_INPUTS "${AUTOCODER_GENERATED_AUTOCODER_INPUTS_VALUES}" PARENT_SCOPE)
    set(AUTOCODER_DEPENDENCIES "${AUTOCODER_DEPENDENCIES_VALUES}" PARENT_SCOPE)
    set(AUTOCODER_GENERATED_OTHER "${AUTOCODER_GENERATED_OTHER_VALUES}" PARENT_SCOPE)
endfunction()

####
# run_ac:
#
# Run the autocoder across the set of source files, SOURCES, and the previously generated sources, GENERATED_SOURCES.
# This will filter the SOURCES and GENERATED_SOURCES down to the handled set. Then for single-input autocoders, it runs
# the autocoder one input at a time, otherwise it runs the autocoder once on all inputs.
#
# AUTOCODER_CMAKE: cmake file containing autocoder definition
# SOURCES: sources input to run on the autocoder
####
function(run_ac BUILD_TARGET_NAME AUTOCODER_CMAKE SOURCES GENERATED_FILE_LIST HASH)
    plugin_include_helper(AUTOCODER_NAME "${AUTOCODER_CMAKE}" is_supported setup_autocode get_generated_files get_dependencies)
    # Normalize and filter source paths so that what we intend to run is in a standard form
    normalize_paths(AC_INPUT_SOURCES "${SOURCES}")
    _filter_sources(AC_INPUT_SOURCES "${AC_INPUT_SOURCES}")

    # Break early if there are no sources, no need to autocode nothing
    if (NOT AC_INPUT_SOURCES)
        if (CMAKE_DEBUG_OUTPUT)
            message(STATUS "[Autocode/${AUTOCODER_NAME}] No sources detected")
        endif()
        return()
    endif()

    # Check if this autocoder has been run before by comparing the hash of inputs. This allows us to skip running
    # at a previous time.  If so, skip autocoder and use old results
    string(SHA1 "SRCS_HASH" "${AC_INPUT_SOURCES};${AUTOCODER_CMAKE}")
    
    # Check if we have a previously stored hash and compare it
    get_property(STORED_HASH TARGET "${BUILD_TARGET_NAME}" PROPERTY "${AUTOCODER_NAME}_SRCS_HASH")
    get_property(HASH_SET TARGET "${BUILD_TARGET_NAME}" PROPERTY "${AUTOCODER_NAME}_SRCS_HASH" SET)
    
    # If we have not run this autocoder before, or if the hash has changed, run the autocoder
    if (NOT HASH_SET)
        # Store the hash for future runs
        set_property(TARGET "${BUILD_TARGET_NAME}" PROPERTY "${AUTOCODER_NAME}_SRCS_HASH" "${SRCS_HASH}")
        
        _describe_autocoder_prep("${AUTOCODER_NAME}" "${AC_INPUT_SOURCES}")

        # Find the one variable set in the autocoder
        get_property(HANDLES_INDIVIDUAL_SOURCES_SET GLOBAL PROPERTY "${AUTOCODER_NAME}_HANDLES_INDIVIDUAL_SOURCES" SET)
        if (NOT HANDLES_INDIVIDUAL_SOURCES_SET)
            message(FATAL_ERROR "${AUTOCODER_CMAKE} did not call one of the autocoder_setup_for_*_sources functions")
        endif()
        get_property(HANDLES_INDIVIDUAL_SOURCES GLOBAL PROPERTY "${AUTOCODER_NAME}_HANDLES_INDIVIDUAL_SOURCES")

        # Handles individual/multiple source handling
        if (HANDLES_INDIVIDUAL_SOURCES)
            foreach(SOURCE IN LISTS AC_INPUT_SOURCES)
                __ac_process_sources("${BUILD_TARGET_NAME}" "${SOURCE}")
            endforeach()
        else()
            __ac_process_sources("${BUILD_TARGET_NAME}" "${AC_INPUT_SOURCES}")
        endif()
    else()
        # Assert runs are identical for the same autocoder
        fprime_cmake_ASSERT("Hash mismatch for autocoder ${AUTOCODER_NAME}: stored '${STORED_HASH}' vs calculated '${SRCS_HASH}'" 
                            "${STORED_HASH}" STREQUAL "${SRCS_HASH}")
    endif()

    # Read autocoder outputs from properties using the centralized variable lists
    set(ALL_AUTOCODER_VARIABLES ${FPRIME_AUTOCODER_REQUIRED} ${FPRIME_AUTOCODER_OPTIONAL} AUTOCODER_GENERATED)

    # Process each autocoder variable and append to target properties with the same name
    foreach(VARIABLE_NAME IN LISTS ALL_AUTOCODER_VARIABLES)
        get_property(VARIABLE_VALUES TARGET "${BUILD_TARGET_NAME}" PROPERTY "${AUTOCODER_NAME}_${VARIABLE_NAME}")
        if (VARIABLE_VALUES)
            append_list_property("${VARIABLE_VALUES}" TARGET "${BUILD_TARGET_NAME}" PROPERTY "${HASH}_${VARIABLE_NAME}")
        endif()
    endforeach()
    _describe_autocoder_run("${AUTOCODER_NAME}")

endfunction(run_ac)

####
# Function `_describe_autocoder_prep`:
#
# Describes the inputs into an autocoder run. Does nothing unless CMAKE_DEBUG_OUTPUT is ON. Run before running the
# autocoder
#
# AUTOCODER_NAME: name of autocoder being run
# AC_INPUT_SOURCES: input files to autocoder
####
function(_describe_autocoder_prep AUTOCODER_NAME AC_INPUT_SOURCES)
    # Start by displaying inputs to autocoders
    if (CMAKE_DEBUG_OUTPUT)
        message(STATUS "[Autocode/${AUTOCODER_NAME}] Autocoding Input Sources:")
        foreach(SOURCE IN LISTS AC_INPUT_SOURCES)
            message(STATUS "[Autocode/${AUTOCODER_NAME}]   ${SOURCE}")
        endforeach()
    endif()
endfunction()

####
# Function `_describe_autocoder_run`:
#
# Describe the results of an autocoder run. Does nothing unless CMAKE_DEBUG_OUTPUT is ON. Must have run the autocoder
# already and set the properties.
#
# AUTOCODER_NAME: name of autocoder being described
####
function(_describe_autocoder_run AUTOCODER_NAME)
    # When actually generating items, explain what is done and why
    if (CMAKE_DEBUG_OUTPUT)
        # Create a map of property names to display names
        set(PROPERTY_DISPLAY_NAMES)
        list(APPEND PROPERTY_DISPLAY_NAMES "AUTOCODER_GENERATED" "Generated Files")
        list(APPEND PROPERTY_DISPLAY_NAMES "AUTOCODER_GENERATED_BUILD_SOURCES" "New Build Sources")
        list(APPEND PROPERTY_DISPLAY_NAMES "AUTOCODER_GENERATED_AUTOCODER_INPUTS" "Additional Autocode Inputs")
        list(APPEND PROPERTY_DISPLAY_NAMES "AUTOCODER_GENERATED_OTHER" "Other Generated Files")
        list(APPEND PROPERTY_DISPLAY_NAMES "AUTOCODER_DEPENDENCIES" "Module Dependencies")
        
        # Process all variables from the FPRIME_AUTOCODER lists plus AUTOCODER_GENERATED
        set(ALL_AUTOCODER_VARIABLES ${FPRIME_AUTOCODER_REQUIRED} ${FPRIME_AUTOCODER_OPTIONAL} AUTOCODER_GENERATED)
        
        foreach(VARIABLE_NAME IN LISTS ALL_AUTOCODER_VARIABLES)
            # Find the display name for this variable
            list(FIND PROPERTY_DISPLAY_NAMES "${VARIABLE_NAME}" NAME_INDEX)
            if (NAME_INDEX GREATER_EQUAL 0)
                math(EXPR DISPLAY_INDEX "${NAME_INDEX} + 1")
                list(GET PROPERTY_DISPLAY_NAMES ${DISPLAY_INDEX} DISPLAY_NAME)
                
                get_property(PROPERTY_VALUES TARGET "${BUILD_TARGET_NAME}" PROPERTY "${AUTOCODER_NAME}_${VARIABLE_NAME}")
                if (PROPERTY_VALUES)
                    message(STATUS "[Autocode/${AUTOCODER_NAME}] ${DISPLAY_NAME}:")
                    foreach(VALUE IN LISTS PROPERTY_VALUES)
                        message(STATUS "[Autocode/${AUTOCODER_NAME}]   ${VALUE}")
                    endforeach()
                endif()
            endif()
        endforeach()
    endif()
endfunction()

####
# _filter_sources:
#
# Filters sources down to the ones supported by the active autocoder. It is an error to call this helper function before
# including an autocoder's CMake file and thus setting the active autocoder. Helper function.
#
# OUTPUT_NAME: name of output variable to set in parent scope
# ...: any number of arguments containing lists of sources
####
function(_filter_sources OUTPUT_NAME)
    set(OUTPUT_LIST)
    # Loop over the list and check
    foreach (SOURCE_LIST IN LISTS ARGN)
        foreach(SOURCE IN LISTS SOURCE_LIST)
            cmake_language(CALL "${AUTOCODER_NAME}_is_supported" "${SOURCE}")
            if (IS_SUPPORTED)
                list(APPEND OUTPUT_LIST "${SOURCE}")
            endif()
        endforeach()
    endforeach()
    set(${OUTPUT_NAME} "${OUTPUT_LIST}" PARENT_SCOPE)
endfunction(_filter_sources)

####
# __ac_process_sources:
#
# Process sources found in SOURCES list and sets up the autocoder to run on the sources by registering a rule to create
# those sources.
# SOURCES: source file list. Note: if the autocoder sets HANDLES_INDIVIDUAL_SOURCES this will be singular
####
function(__ac_process_sources BUILD_TARGET_NAME SOURCES)
    # Loop through the variables from the various lists and make sure they are undefined
    foreach(VARIABLE IN LISTS FPRIME_AUTOCODER_UNSUPPORTED FPRIME_AUTOCODER_REQUIRED FPRIME_AUTOCODER_OPTIONAL)
        fprime_cmake_ASSERT("'${VARIABLE}' set to '${${VARIABLE}}' before call" NOT DEFINED ${VARIABLE})
    endforeach()
    # Run the generation setup when not requesting "info only"
    cmake_language(CALL "${AUTOCODER_NAME}_setup_autocode" "${BUILD_TARGET_NAME}" "${SOURCES}")

    # Check for removed support
    foreach(VARIABLE IN LISTS FPRIME_AUTOCODER_UNSUPPORTED)
        fprime_cmake_ASSERT("Unsupported '${VARIABLE}' set to '${${VARIABLE}}' by autocoder ${AUTOCODER_NAME}"
                            NOT DEFINED ${VARIABLE})
    endforeach()

    # Search through requirements ensuring one was set
    set(AUTOCODER_MET_REQUIRED FALSE)
    foreach(REQUIREMENT IN LISTS FPRIME_AUTOCODER_REQUIRED)
        if (DEFINED ${REQUIREMENT})
            list(APPEND AUTOCODER_GENERATED ${${REQUIREMENT}})
            set(AUTOCODER_MET_REQUIRED TRUE)
        endif()
    endforeach()
    fprime_cmake_ASSERT("Autocoder must define at least one of: ${FPRIME_AUTOCODER_REQUIRED}" AUTOCODER_MET_REQUIRED)

    # Set autocoder output variables as target properties
    foreach(VARIABLE IN LISTS FPRIME_AUTOCODER_REQUIRED FPRIME_AUTOCODER_OPTIONAL)
        if (DEFINED ${VARIABLE})
            append_list_property("${${VARIABLE}}" TARGET "${BUILD_TARGET_NAME}" PROPERTY "${AUTOCODER_NAME}_${VARIABLE}")
        endif()
    endforeach()
    # Also set the calculated AUTOCODER_GENERATED
    if (DEFINED AUTOCODER_GENERATED)
        append_list_property("${AUTOCODER_GENERATED}" TARGET "${BUILD_TARGET_NAME}" PROPERTY "${AUTOCODER_NAME}_AUTOCODER_GENERATED")
    endif()
endfunction()
