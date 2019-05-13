####
# Utils.cmake:
#
# Defines needed utility functions that are (or should be) independent of any autocoding calls to
# be made. Those files should go in *AC_Utils.cmake*.
####

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
  if (EXISTS ${DIRECTORY_PATH})
      # Get path name relative to the root directory
      file(RELATIVE_PATH TEMP_MODULE_NAME ${FPRIME_CURRENT_BUILD_ROOT} ${DIRECTORY_PATH})
  else()
      set(TEMP_MODULE_NAME ${DIRECTORY_PATH})
  endif()
  # Replace slash with underscore to have valid name
  string(REPLACE "/" "_" TEMP_MODULE_NAME ${TEMP_MODULE_NAME})
  if (DEFINED BUILD_SUFFIX)
      set(TEMP_MODULE_NAME "${TEMP_MODULE_NAME}${BUILD_SUFFIX}")
  endif()
  set(MODULE_NAME ${TEMP_MODULE_NAME} PARENT_SCOPE)
endfunction(get_module_name)

####
# Function `setup_module_dicts`:
#
# Creates a dictionary target for the module, that is then added to the "dict" and "module"
# targets.
#
# - **MOD_NAME:** name of module being processed
# - **AI_XML:** AI_XML that is generating dictionaries
# - **DICT_INPUTS:** inputs from auto-coder, used to trigger dictionary generation
####
function(setup_module_dicts MOD_NAME AI_XML DICT_INPUTS)
    # UTs don't supply directories
    if (UT_BUILD)
        return()
    endif()
	set(AI_DICT_NAME "${AI_XML}_dict")
	set(MOD_DICT_NAME "${MOD_NAME}_dict")
	# Add the dictionary target for this module, if it doesn't already exist
	if (NOT TARGET ${MOD_DICT_NAME})
	    add_custom_target(${MOD_DICT_NAME})
		if (CMAKE_DEBUG_OUTPUT)
		    message(STATUS "\tAdding Dict Target: ${MOD_NAME}_dict")
		endif()
	endif()
	add_custom_target(${AI_DICT_NAME} DEPENDS ${DICT_INPUTS})
	# Add dependencies upstream
	add_dependencies(${AI_DICT_NAME} ${CODEGEN_TARGET})
	add_dependencies(${MOD_DICT_NAME} ${AI_DICT_NAME})
endfunction(setup_module_dicts)

####
# Function `add_dict_deps`:
#
# Used to track dictionary dependencies, in order to ensure that the fewest number of dictionary
# targets are used.
#
# - **MODULE_NAME:** module receiving a dict dependency
# - **DEP_MODULE_NAME:** name of the module whose dictionary will be added
####
function(add_dict_deps MODULE_NAME DEP_MODULE_NAME)
    # UTs don't do dictionaries
    if (UT_BUILD)
        return()
    endif()
    # Skip if there is no dict module to be added
    if (TARGET "${DEP_MODULE_NAME}_DICT")
        # We have sub-dictionaries, create a roll-up target
        if (NOT TARGET "${MODULE_NAME}_DICT")
            if (CMAKE_DEBUG_OUTPUT)
                message(STATUS "\tAdding Faux-Dict Target: ${MODULE_NAME}_DICT")
            endif()
            add_custom_target("${MODULE_NAME}_DICT")
        endif()
        add_dependencies("${MODULE_NAME}_DICT" "${DEP_MODULE_NAME}_DICT")
    endif()
endfunction(add_dict_deps)

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
# Function `fprime_dependencies`:
#
# A function used to detect the dependencies of a given module from the XML file that
# defines this module. This is used to reduce code in the Serializable, Port, Component,
# and Topology functions that all use the same procedure.
#
# - **XML_PATH:** full path to the XML used for sources.
# - **MODULE_NAME:** name of the module soliciting new dependencies
# - **PARSER_TYPE:** type of parser to use. Must be one of the prefixes *_xml in cmake/parser/
####
function(fprime_dependencies XML_PATH MODULE_NAME PARSER_TYPE)
  if (NOT ${BUILD_SUFFIX} STREQUAL "")
      string(REGEX REPLACE "${BUILD_SUFFIX}" "" MODULE_NAME_NO_SUFFIX "${MODULE_NAME}")
  else()
      set(MODULE_NAME_NO_SUFFIX "${MODULE_NAME}")
  endif()
  execute_process(
      COMMAND "${FPRIME_CORE_DIR}/cmake/support/parser/ai_parser.py" "${XML_PATH}" "${MODULE_NAME_NO_SUFFIX}" "${FPRIME_CURRENT_BUILD_ROOT}"
	  RESULT_VARIABLE ERR_RETURN
	  OUTPUT_VARIABLE TARGETS
  )
  # Check parser return code
  if(ERR_RETURN)
     message(FATAL_ERROR "Failed to parse ${XML_PATH}. ${ERR_RETURN}")
  endif()
  # For every dected dependency, add them to the supplied module. This enforces build order.
  # Also set the link dependencies on this module. CMake rolls-up link dependencies, and thus
  # this prevents the need for manually specifying link orders.
  foreach(TARGET ${TARGETS})
    add_dependencies(${MODULE_NAME} "${TARGET}${BUILD_SUFFIX}")
    target_link_libraries(${MODULE_NAME} "${TARGET}${BUILD_SUFFIX}")
    add_dict_deps(${MODULE_NAME}  "${TARGET}${BUILD_SUFFIX}")
  endforeach()
endfunction(fprime_dependencies)

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
        if (INPUTFILE MATCHES ".*\.xml$" OR INPUTFILE MATCHES ".*\.txt")
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
# Function `print_dependencies`:
#
# Prints the dependency list of the module supplied as well as the include directories.
#
# - **MODULE_NAME**: module name to print dependencies fors
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
