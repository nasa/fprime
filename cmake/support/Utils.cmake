####
# Utils.cmake:
#
# Defines needed utility functions that are (or should be) independent of any autocoding calls to
# be made. Those files should go in *AC_Utils.cmake*.
####

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
# Function `add_generated_sources`:
#
# Add generated file as sources for the given target. This function specifically adds
# these sources as generated, so that regardless of where they appear (source or build tree)
# they will be known as generated
#
# - **CPP_SOURCE:** source .cpp file to add
# - **HPP_SOURCE:** source .hpp file to add
# - **MODULE_NAME:** (inherited from parent) name of the module to add generated sources too
####
function(add_generated_sources CPP_SOURCE HPP_SOURCE)
  if (CMAKE_DEBUG_OUTPUT)
    message(STATUS "\tGenerated files: ${CPP_SOURCE} ${HPP_SOURCE}")
  endif()
  set_source_files_properties(${HPP_SOURCE} PROPERTIES GENERATED TRUE)
  # Add auto-coded sources to the module as sources
  target_sources(
    ${MODULE_NAME}
    PRIVATE ${CPP_SOURCE} ${HPP_SOURCE}
  )
  # Set those files as generated to prevent build errors
  set_source_files_properties(${CPP_SOURCE} PROPERTIES GENERATED TRUE)
  set_source_files_properties(${HPP_SOURCE} PROPERTIES GENERATED TRUE)
  # Includes the source, so that the Ac files can include source headers
  target_include_directories("${MODULE_NAME}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endfunction(add_generated_sources)

####
# Function `fprime_ai_info`:
#
# A function used to detect all the needed information for an Ai.xml file. This looks for the following items:
#  1. Type of object defined inside: Component, Port, Enum, Serializable, TopologyApp
#  2. All fprime module dependencies that may be auto-detected
#  3. All file dependencies
#
# - **XML_PATH:** full path to the XML used for sources.
# - **MODULE_NAME:** name of the module soliciting new dependencies
####
function(fprime_ai_info XML_PATH MODULE_NAME)
  # Run the parser and capture the output. If an error occurs, that fatals CMake as we cannot continue
  set(MODULE_NAME_NO_SUFFIX "${MODULE_NAME}")
  execute_process(
      COMMAND "${FPRIME_FRAMEWORK_PATH}/cmake/support/parser/ai_parser.py" "${XML_PATH}" "${MODULE_NAME_NO_SUFFIX}" "${FPRIME_CLOSEST_BUILD_ROOT}"
	  RESULT_VARIABLE ERR_RETURN
	  OUTPUT_VARIABLE AI_OUTPUT
  )
  if(ERR_RETURN)
     message(FATAL_ERROR "Failed to parse ${XML_PATH}. ${ERR_RETURN}")
  endif()
  # Next parse the output matching one line at a time, then consuming it and matching the next
  string(REGEX MATCH   "([^\r\n]+)" XML_TYPE "${AI_OUTPUT}")
  string(REGEX REPLACE "([^\r\n]+)\r?\n(.*)" "\\2" AI_OUTPUT "${AI_OUTPUT}")
  string(REGEX MATCH   "^([^\r\n]+)" MODULE_DEPENDENCIES "${AI_OUTPUT}")
  string(REGEX REPLACE "([^\r\n]+)\r?\n(.*)" "\\2" AI_OUTPUT "${AI_OUTPUT}")
  string(REGEX MATCH   "^([^\r\n]+)" FILE_DEPENDENCIES "${AI_OUTPUT}")

  # Next compute the needed variants of the items needed. This
  string(TOLOWER ${XML_TYPE} XML_LOWER_TYPE)
  get_filename_component(XML_NAME "${INPUT_FILE}" NAME)
  string(REGEX REPLACE "(${XML_TYPE})?Ai.xml" "" AC_OBJ_NAME "${XML_NAME}")

  # Finally, set all variables into parent scope
  set(XML_TYPE "${XML_TYPE}" PARENT_SCOPE)
  set(XML_LOWER_TYPE "${XML_LOWER_TYPE}" PARENT_SCOPE)
  set(AC_OBJ_NAME "${AC_OBJ_NAME}" PARENT_SCOPE)
  set(MODULE_DEPENDENCIES "${MODULE_DEPENDENCIES}" PARENT_SCOPE)
  set(FILE_DEPENDENCIES "${FILE_DEPENDENCIES}" PARENT_SCOPE)
endfunction(fprime_ai_info)

####
# Function `split_source_files`:
#
# Split SOURCE_FILE_INPUTS into the autocoder and source variants, supplied to the calling function.
#
# - **SOURCE_FILE_INPUTS**: input file list to be split up.
# - **Return: AUTOCODER_INPUT_FILES** (set in parent scope)
# - **Return: SOURCE_FILES** (set in parent scope)
####
function(split_source_files SOURCE_INPUT_FILES)
    set(AC "")
    set(SC "")
    foreach (INPUTFILE ${SOURCE_INPUT_FILES})
        if (INPUTFILE MATCHES ".*\\.xml$" OR INPUTFILE MATCHES ".*\\.txt")
            list(APPEND AC ${INPUTFILE})
        else()
            list(APPEND SC ${INPUTFILE})
        endif()
    endforeach()
	# Return the variables to calling scope
	set(AUTOCODER_INPUT_FILES "${AC}" PARENT_SCOPE)
	set(SOURCE_FILES "${SC}" PARENT_SCOPE)
endfunction(split_source_files)

####
# Function `split_dependencies`:
#
# Split DEPS_INPUT into the module and link variants, supplied to the calling function.
#
# - **DEPS_INPUT**: dependencies list to be split up.
# - **Return: LINK_DEPS** (set in parent scope)
# - **Return: MOD_DEPS** (set in parent scope)
####
function(split_dependencies DEPS_INPUT)
    set(LD "")
    set(FD "")
    foreach (INPUTFILE ${DEPS_INPUT})
        if (INPUTFILE MATCHES "^-l.*")
            list(APPEND LD ${INPUTFILE})
        else()
            list(APPEND FD ${INPUTFILE})
        endif()
    endforeach()
	# Return the variables to calling scope
	set(LINK_DEPS "${LD}" PARENT_SCOPE)
	set(MOD_DEPS "${FD}" PARENT_SCOPE)
endfunction(split_dependencies)
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
# Function `print_dependencies`:
#
# Prints the dependency list of the module supplied as well as the include directories.
#
# - **MODULE_NAME**: module name to print dependencies
####
function(print_dependencies MODULE_NAME)
     get_target_property(OUT "${MODULE_NAME}" INCLUDE_DIRECTORIES)
     if (OUT MATCHES ".*-NOTFOUND")
       set(OUT "--none--")
     endif()
     message(STATUS "\tInclude Directories: ${OUT}")
     get_target_property(OUT "${MODULE_NAME}" LINK_LIBRARIES)
     if (OUT MATCHES ".*-NOTFOUND")
       set(OUT "--none--")
     endif()
     message(STATUS "\tLinks dependencies: ${OUT}")
endfunction(print_dependencies)
