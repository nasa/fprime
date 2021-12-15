####
# init_variables:
#
# Initialize all variables passed in to empty variables in the calling scope.
####
function(init_variables)
    foreach (VARIABLE IN LISTS ARGN)
        set(${VARIABLE} PARENT_SCOPE)
    endforeach()
endfunction(init_variables)


####
# normalize_paths:
#
# Take in any number of lists of paths and normalize the paths returning a single list.
# OUTPUT_NAME: name of variable to set in parent scope
####
function(normalize_paths OUTPUT_NAME)
    set(OUTPUT_LIST)
    # Loop over the list and check
    foreach (PATH_LIST IN LISTS ARGN)
        foreach(PATH IN LISTS PATH_LIST)
            get_filename_component(PATH "${PATH}" REALPATH)
            list(APPEND OUTPUT_LIST "${PATH}")
        endforeach()
    endforeach()
    set(${OUTPUT_NAME} "${OUTPUT_LIST}" PARENT_SCOPE)
endfunction(normalize_paths)

####
# resolve_dependencies:
#
# Sets OUTPUT_VAR in parent scope to be the set of dependencies in canonical form: relative path from root replacing
# directory separators with "_".  E.g. fprime/Fw/Time becomes Fw_Time.
#
# OUTPUT_VAR: variable to fill in parent scope
# ARGN: list of dependencies to resolve
####
function(resolve_dependencies OUTPUT_VAR)
    # Resolve all dependencies
    set(RESOLVED)
    foreach(DEPENDENCY IN LISTS ARGN)
        get_module_name(${DEPENDENCY})
        if (NOT MODULE_NAME IN_LIST RESOLVED)
            list(APPEND RESOLVED "${MODULE_NAME}")
        endif()
    endforeach()
    set(${OUTPUT_VAR} "${RESOLVED}" PARENT_SCOPE)
endfunction(resolve_dependencies)


####
# build_relative_path:
#
# Calculate the path to an item relative to known build paths.  Search is performed in the following order erring if the
# item is found in multiple paths.
#
# INPUT_PATH: input path to search
# OUTPUT_VAR: output variable to fill
####
function(build_relative_path INPUT_PATH OUTPUT_VAR)
    # Implementation assertion
    if (NOT DEFINED FPRIME_BUILD_LOCATIONS)
        message(FATAL_ERROR "FPRIME_BUILD_LOCATIONS not set before build_relative_path was called")
    endif()
    normalize_paths(FPRIME_LOCS_NORM ${FPRIME_BUILD_LOCATIONS})
    normalize_paths(INPUT_PATH ${INPUT_PATH})
    foreach(PARENT IN LISTS FPRIME_LOCS_NORM)
        string(REGEX REPLACE "${PARENT}/(.*)$" "\\1" LOC_TEMP "${INPUT_PATH}")
        if (NOT LOC_TEMP STREQUAL INPUT_PATH AND NOT LOC_TEMP MATCHES "${LOC}$")
            message(FATAL_ERROR "Found ${INPUT_PATH} at multiple locations: ${LOC} and ${LOC_TEMP}")
        elseif(NOT LOC_TEMP STREQUAL INPUT_PATH AND NOT DEFINED LOC)
            set(LOC "${LOC_TEMP}")
        endif()
    endforeach()
    if (LOC STREQUAL "")
        message(FATAL_ERROR "Failed to find location for: ${INPUT_PATH}")
    endif()
    set(${OUTPUT_VAR} ${LOC} PARENT_SCOPE)
endfunction(build_relative_path)

####
# on_any_changed:
#
# Sets VARIABLE to true if any file has been noted as changed from the "on_changed" function.  Will create cache files
# in the binary directory.  Please see: on_changed
#
# INPUT_FILES: files to check for changes
# ARGN: passed into execute_process via on_changed call
####
function (on_any_changed INPUT_FILES VARIABLE)
    foreach(INPUT_FILE IN LISTS INPUT_FILES)
        on_changed("${INPUT_FILE}" TEMP_ON_CHANGED ${ARGN})
        if (TEMP_ON_CHANGED)
            set(${VARIABLE} TRUE PARENT_SCOPE)
            return()
        endif()
    endforeach()
    set(${VARIABLE} FALSE PARENT_SCOPE)
endfunction()

####
# on_changed:
#
# Sets VARIABLE to true if and only if the given file has changed since the last time this function was invoked. It will
# create "${INPUT_FILE}.prev" in the binary directory as a cache from the previous invocation. The result is always TRUE
# unless a successful no-difference is calculated.
#
# INPUT_FILE: file to check if it has changed
# ARGN: passed into execute_process
####
function (on_changed INPUT_FILE VARIABLE)
    get_filename_component(INPUT_BASENAME "${INPUT_FILE}" NAME)
    set(PREVIOUS_FILE "${CMAKE_CURRENT_BINARY_DIR}/${INPUT_BASENAME}.prev")

    execute_process(COMMAND "${CMAKE_COMMAND}" -E compare_files "${INPUT_FILE}" "${PREVIOUS_FILE}"
                    RESULT_VARIABLE difference OUTPUT_QUIET ERROR_QUIET)
    # Files are the same, leave this function
    if (difference EQUAL 0)
        set(${VARIABLE} FALSE PARENT_SCOPE)
        return()
    endif()
    set(${VARIABLE} TRUE PARENT_SCOPE)
    # Update the file with the latest
    if (EXISTS "${INPUT_FILE}")
        execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${INPUT_FILE}" "${PREVIOUS_FILE}" OUTPUT_QUIET)
    endif()
endfunction()

####
# read_from_lines:
#
# Reads a set of variables from a newline delimited test base. This will read each variable as a separate line. It is
# based on the number of arguments passed in.
####
function (read_from_lines CONTENT)
    # Loop through each arg
    foreach(NAME IN LISTS ARGN)
        string(REGEX MATCH   "^([^\r\n]+)" VALUE "${CONTENT}")
        string(REGEX REPLACE "^([^\r\n]*)\r?\n(.*)" "\\2" CONTENT "${CONTENT}")
        set(${NAME} "${VALUE}" PARENT_SCOPE)
    endforeach()
endfunction()


####
# Function `get_nearest_build_root`:
#
# Finds the nearest build root from ${FPRIME_BUILD_LOCATIONS} that is a parent of DIRECTORY_PATH.
#
# - **DIRECTORY_PATH:** path to detect nearest build root
# Return: nearest parent from ${FPRIME_BUILD_LOCATIONS}
####
function(get_nearest_build_root DIRECTORY_PATH)
    set(FOUND_BUILD_ROOT "${DIRECTORY_PATH}")
    set(LAST_REL "${DIRECTORY_PATH}")
    foreach(FPRIME_BUILD_LOC ${FPRIME_BUILD_LOCATIONS})
        file(RELATIVE_PATH TEMP_MODULE ${FPRIME_BUILD_LOC} ${DIRECTORY_PATH})
        string(LENGTH "${LAST_REL}" LEN1)
        string(LENGTH "${TEMP_MODULE}" LEN2)
        if (LEN2 LESS LEN1 AND TEMP_MODULE MATCHES "^[^./].*")
            set(FOUND_BUILD_ROOT "${FPRIME_BUILD_LOC}")
            set(LAST_REL "${TEMP_MODULE}")
        endif()
    endforeach()
    if ("${FOUND_BUILD_ROOT}" STREQUAL "${DIRECTORY_PATH}")
        message(FATAL_ERROR "No build root found for: ${DIRECTORY_PATH}")
    endif()
    set(FPRIME_CLOSEST_BUILD_ROOT "${FOUND_BUILD_ROOT}" PARENT_SCOPE)
endfunction()
####
# Function `get_module_name`:
#
# Takes a path, or something path-like and returns the module's name. This breaks down as the
# following:
#
#  1. If passed a path, the module name is the '_'ed variant of the relative path from BUILD_ROOT
#  2. If passes something which does not exist on the file system, it is just '_'ed
#
# i.e. ${BUILD_ROOT}/Svc/ActiveLogger becomes Svc_ActiveLogger
#      Svc/ActiveLogger also becomes Svc_ActiveLogger
#
# - **DIRECTORY_PATH:** path to infer MODULE_NAME from
# - **Return: MODULE_NAME** (set in parent scope)
####
function(get_module_name DIRECTORY_PATH)
    # If DIRECTORY_PATH exists, then find its offset from BUILD_ROOT to calculate the module
    # name. If it does not exist, then it is assumed to be an offset already and is carried
    # forward in the calculation.
    if (EXISTS ${DIRECTORY_PATH} AND IS_ABSOLUTE ${DIRECTORY_PATH})
        # Module names a based on the current directory, not a file
        if (NOT IS_DIRECTORY ${DIRECTORY_PATH})
            get_filename_component(DIRECTORY_PATH "${DIRECTORY_PATH}" DIRECTORY)
        endif()
        # Get path name relative to the root directory
        get_nearest_build_root(${DIRECTORY_PATH})
        File(RELATIVE_PATH TEMP_MODULE_NAME ${FPRIME_CLOSEST_BUILD_ROOT} ${DIRECTORY_PATH})
    else()
        set(TEMP_MODULE_NAME ${DIRECTORY_PATH})
    endif()
    # Replace slash with underscore to have valid name
    string(REPLACE "/" "_" TEMP_MODULE_NAME ${TEMP_MODULE_NAME})
    set(MODULE_NAME ${TEMP_MODULE_NAME} PARENT_SCOPE)
endfunction(get_module_name)


####
# Function `set_hash_flag`:
#
# Adds a -DASSERT_FILE_ID=(First 8 digits of MD5) to each source file, and records the output in
# hashes.txt. This allows for asserts on file ID not string.
####
function(set_hash_flag SRC)
    get_filename_component(FPRIME_CLOSEST_BUILD_ROOT_ABS "${FPRIME_CLOSEST_BUILD_ROOT}" ABSOLUTE)
    string(REPLACE "${FPRIME_CLOSEST_BUILD_ROOT_ABS}/" "" SHORT_SRC "${SRC}")
    string(MD5 HASH_VAL "${SHORT_SRC}")
    string(SUBSTRING "${HASH_VAL}" 0 8 HASH_32)
    file(APPEND "${CMAKE_BINARY_DIR}/hashes.txt" "${SHORT_SRC}: 0x${HASH_32}\n")
    SET_SOURCE_FILES_PROPERTIES(${SRC} PROPERTIES COMPILE_FLAGS -DASSERT_FILE_ID="0x${HASH_32}")
endfunction(set_hash_flag)


####
# Function `print_property`:
#
# Prints a given property for the module.
# - **TARGET**: target to print properties
# - **PROPERTY**: name of property to print
####
function (print_property TARGET PROPERTY)
    get_target_property(OUT "${TARGET}" "${PROPERTY}")
    if (NOT OUT MATCHES ".*-NOTFOUND")
        message(STATUS "[F´ Module] ${TARGET} ${PROPERTY}:")
        foreach (PROPERTY IN LISTS OUT)
            message(STATUS "[F´ Module]    ${PROPERTY}")
        endforeach()
    endif()
endfunction(print_property)

####
# Function `introspect`:
#
# Prints the dependency list of the module supplied as well as the include directories.
#
# - **MODULE_NAME**: module name to print dependencies
####
function(introspect MODULE_NAME)
    print_property("${MODULE_NAME}" SOURCES)
    print_property("${MODULE_NAME}" INCLUDE_DIRECTORIES)
    print_property("${MODULE_NAME}" LINK_LIBRARIES)
endfunction(introspect)
