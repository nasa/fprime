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

####
# run_ac_set:
#
# Run a set of autocoder allowing back-to-back execution of a set of autocoders. SOURCES are the source files that are
# input into the autocoder filters.  Extra arguments may be an include path for an autocoder (e.g. autocoder/fpp).
#
# SOURCES: source file input list
# ...: autocoder include
####
function (run_ac_set SOURCES)
    # Get the source list, if passed in
    set(AC_LIST)
    if (ARGN)
        set(AC_LIST "${ARGN}")
    endif()
    init_variables(MODULE_DEPENDENCIES_LIST GENERATED_FILE_LIST CONSUMED_SOURCES_LIST)
    foreach(AC_CMAKE IN LISTS AC_LIST)
        init_variables(MODULE_DEPENDENCIES GENERATED_FILES CONSUMED_SOURCES)
        run_ac("${AC_CMAKE}" "${SOURCES}" "${GENERATED_FILE_LIST}")
        list(APPEND MODULE_DEPENDENCIES_LIST ${MODULE_DEPENDENCIES})
        list(APPEND GENERATED_FILE_LIST ${GENERATED_FILES})
        list(APPEND CONSUMED_SOURCES_LIST ${CONSUMED_SOURCES})
    endforeach()

    # Return variables
    set(AC_DEPENDENCIES "${MODULE_DEPENDENCIES_LIST}" PARENT_SCOPE)
    set(AC_GENERATED "${GENERATED_FILE_LIST}" PARENT_SCOPE)
    set(AC_SOURCES "${CONSUMED_SOURCES_LIST}" PARENT_SCOPE)
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
# GENERATED_SOURCES: sources created by other autocoders
####
function(run_ac AUTOCODER_CMAKE SOURCES GENERATED_SOURCES)
    plugin_include_helper(AUTOCODER_NAME "${AUTOCODER_CMAKE}" is_supported setup_autocode get_generated_files get_dependencies)
    # Normalize and filter source paths so that what we intend to run is in a standard form
    normalize_paths(AC_INPUT_SOURCES "${SOURCES}" "${GENERATED_SOURCES}")
    _filter_sources(AC_INPUT_SOURCES "${GENERATED_SOURCES}" "${AC_INPUT_SOURCES}")

    # Break early if there are no sources, no need to autocode nothing
    if (NOT AC_INPUT_SOURCES)
        if (CMAKE_DEBUG_OUTPUT)
            message(STATUS "[Autocode/${AUTOCODER_NAME}] No sources detected")
        endif()
        return()
    endif()
    string(SHA1 "SRCS_HASH" "${AC_INPUT_SOURCES};${AUTOCODER_CMAKE}")

    get_property(DEP_SET DIRECTORY PROPERTY "${SRCS_HASH}_DEPENDENCIES" SET)
    get_property(GEN_SET DIRECTORY PROPERTY "${SRCS_HASH}_GENERATED" SET)
    get_property(CON_SET DIRECTORY PROPERTY "${SRCS_HASH}_CONSUMED" SET)


    # If we have not set these properties, run the autocoder setup function
    if (NOT DEP_SET AND NOT GEN_SET AND NOT CON_SET)
        _describe_autocoder_prep("${AUTOCODER_NAME}" "${AC_INPUT_SOURCES}")

        # Find the one variable set in the autocoder
        get_property(HANDLES_INDIVIDUAL_SOURCES_SET GLOBAL PROPERTY "${AUTOCODER_NAME}_HANDLES_INDIVIDUAL_SOURCES" SET)
        if (NOT HANDLES_INDIVIDUAL_SOURCES_SET)
            message(FATAL_ERROR "${AUTOCODER_CMAKE} did not call one of the autocoder_setup_for_*_sources functions")
        endif()
        get_property(HANDLES_INDIVIDUAL_SOURCES GLOBAL PROPERTY "${AUTOCODER_NAME}_HANDLES_INDIVIDUAL_SOURCES")
        set(CONSUMED_SOURCES)
        # Handles individual/multiple source handling
        if (HANDLES_INDIVIDUAL_SOURCES)
            init_variables(MODULE_DEPENDENCIES_LIST GENERATED_FILES_LIST)
            foreach(SOURCE IN LISTS AC_INPUT_SOURCES)
                __ac_process_sources("${SOURCE}")
                list(APPEND MODULE_DEPENDENCIES_LIST ${MODULE_DEPENDENCIES})
                list(APPEND GENERATED_FILES_LIST ${GENERATED_FILES})
                # Check if this would have generated something, if not don't mark the file as used
                if (GENERATED_FILES)
                    list(APPEND CONSUMED_SOURCES "${SOURCE}")
                endif()
            endforeach()
            set(MODULE_DEPENDENCIES "${MODULE_DEPENDENCIES_LIST}")
            set(GENERATED_FILES "${GENERATED_FILES_LIST}")
        else()
            __ac_process_sources("${AC_INPUT_SOURCES}")
            set(CONSUMED_SOURCES "${AC_INPUT_SOURCES}")
        endif()
        set_property(DIRECTORY PROPERTY "${SRCS_HASH}_DEPENDENCIES" "${MODULE_DEPENDENCIES}")
        set_property(DIRECTORY PROPERTY "${SRCS_HASH}_GENERATED" "${GENERATED_FILES}")
        set_property(DIRECTORY PROPERTY "${SRCS_HASH}_CONSUMED" "${CONSUMED_SOURCES}")
        set_property(DIRECTORY PROPERTY "${SRCS_HASH}_FILE_DEPENDENCIES" "${FILE_DEPENDENCIES}")
        _describe_autocoder_run("${AUTOCODER_NAME}")
    endif()
    get_property(DEPS DIRECTORY PROPERTY "${SRCS_HASH}_DEPENDENCIES")
    get_property(GENS DIRECTORY PROPERTY "${SRCS_HASH}_GENERATED")
    get_property(CONS DIRECTORY PROPERTY "${SRCS_HASH}_CONSUMED")

    # Return variables
    set(MODULE_DEPENDENCIES "${DEPS}" PARENT_SCOPE)
    set(GENERATED_FILES "${GENS}" PARENT_SCOPE)
    set(CONSUMED_SOURCES "${CONS}" PARENT_SCOPE)
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
        get_property(DEPS DIRECTORY PROPERTY "${SRCS_HASH}_DEPENDENCIES")
        get_property(GENS DIRECTORY PROPERTY "${SRCS_HASH}_GENERATED")
        get_property(CONS DIRECTORY PROPERTY "${SRCS_HASH}_CONSUMED")
        get_property(FILES DIRECTORY PROPERTY "${SRCS_HASH}_FILE_DEPENDENCIES")
        message(STATUS "[Autocode/${AUTOCODER_NAME}] Generated Files:")
        foreach(GENERATED_FILE IN LISTS GENS)
            message(STATUS "[Autocode/${AUTOCODER_NAME}]   ${GENERATED_FILE}")
        endforeach()
        # Output file dependency status block
        if (FILES)
            message(STATUS "[Autocode/${AUTOCODER_NAME}] File Dependencies:")
            foreach(FILE_DEPENDENCY IN LISTS FILES)
                message(STATUS "[Autocode/${AUTOCODER_NAME}]   ${FILE_DEPENDENCY}")
            endforeach()
        endif()
        # Output module dependency status block
        if (DEPS)
            message(STATUS "[Autocode/${AUTOCODER_NAME}] Module Dependencies:")
            foreach(MODULE_DEPENDENCY IN LISTS DEPS)
                message(STATUS "[Autocode/${AUTOCODER_NAME}]   ${MODULE_DEPENDENCY}")
            endforeach()
        endif()
    endif()
endfunction()

####
# _filter_sources:
#
# Filters sources down to the ones supported by the active autocoder. It is an error to call this helper function before
# including an autocoder's CMake file and thus setting the active autocoder. Helper function.
#
# OUTPUT_NAME: name of output variable to set in parent scope
# GENERATED_SOURCES: sources created by other autocoders
# ...: any number of arguments containing lists of sources
####
function(_filter_sources OUTPUT_NAME GENERATED_SOURCES)
    set(OUTPUT_LIST)
    # Loop over the list and check
    foreach (SOURCE_LIST IN LISTS ARGN)
        foreach(SOURCE IN LISTS SOURCE_LIST)
            cmake_language(CALL "${AUTOCODER_NAME}_is_supported" "${SOURCE}" "${GENERATED_SOURCES}")
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
function(__ac_process_sources SOURCES)
    # Asserts for consistency
    if (DEFINED AUTOCODER_SCRIPT)
        message(FATAL_ERROR "AUTOCODER_SCRIPT set to ${AUTOCODER_SCRIPT} before setup autocoder call.")
    elseif(DEFINED AUTOCODER_GENERATED)
        message(FATAL_ERROR "AUTOCODER_GENERATED set to ${AUTOCODER_GENERATED} before setup autocoder call.")
    elseif(DEFINED AUTOCODER_INPUTS)
        message(FATAL_ERROR "AUTOCODER_INPUTS set to ${AUTOCODER_INPUTS} before setup autocoder call.")
    elseif(DEFINED AUTOCODER_DEPENDENCIES)
        message(FATAL_ERROR "AUTOCODER_DEPENDENCIES set to ${AUTOCODER_DEPENDENCIES} before setup autocoder call.")
    endif()

    # Run the generation setup when not requesting "info only"
    cmake_language(CALL "${AUTOCODER_NAME}_setup_autocode" "${SOURCES}")

    if (NOT DEFINED AUTOCODER_GENERATED)
        message(FATAL_ERROR "Autocoder ${AUTOCODER_NAME} did not set AUTOCODER_GENERATED to files to be generated")
    elseif(DEFINED AUTOCODER_SCRIPT AND NOT DEFINED AUTOCODER_INPUTS)
        message(FATAL_ERROR "Autocoder ${AUTOCODER_NAME} did not set both AUTOCODER_INPUTS when using AUTOCODER_SCRIPT")
    elseif(DEFINED AUTOCODER_SCRIPT)
        add_custom_command(OUTPUT ${AUTOCODER_GENERATED} COMMAND ${AUTOCODER_SCRIPT} ${AUTOCODER_INPUTS} DEPENDS ${AUTOCODER_INPUTS} ${AUTOCODER_DEPENDENCIES})
    endif()

    set(MODULE_DEPENDENCIES "${AUTOCODER_DEPENDENCIES}" PARENT_SCOPE)
    set(GENERATED_FILES "${AUTOCODER_GENERATED}" PARENT_SCOPE)
    set(FILE_DEPENDENCIES "${AUTOCODER_INPUTS}" PARENT_SCOPE)
endfunction()

