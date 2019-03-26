####
# Get Module Name:
#
# Takes a path, or something path-like and returns the module's name. This breaks down as the
# following:
#
#  1. If passed a path, the module name is the _ed variant of the relative path from BUILD_ROOT
#  2. If passes something which does not exist on the file system, it is just _ed
#
# i.e. ${BUILD_ROOT}/Svc/ActiveLogger becomes Svc_ActiveLogger
#      Svc/ActiveLogger also becomes Svc_ActiveLogger
#
# \param DIRECTORY_PATH: path to infer MODULE_NAME from
# \return MODULE_NAME
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
  set(MODULE_NAME ${TEMP_MODULE_NAME} PARENT_SCOPE)
endfunction(get_module_name)

####
# Generated Files:
#
# Add generated file as sources for the given target. This function specifically adds
# these sources as generated, so that regardless of where they appear (source or build tree)
# they will be known as generated
#
# \param CPP_SOURCE: source .cpp file to add
# \param HPP_SOURCE: source .hpp file to add
# \param MODULE_NAME: name of the module to add generated sources too
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
# FPrime Dependencies:
#
# A function used to detect the dependencies of a given module from the XML file that
# defines this module. This is used to reduce code in the Serializable, Port, Component,
# and Topology functions that all use the same procedure.
#
# \param XML_PATH: full path to the XML used for sources.
# \param MODULE_NAME: name of the module soliciting new dependencies
# \param PARSER_TYPE: type of parser to use. Must be one of the prefixes *_xml in cmake/parser/
####
function(fprime_dependencies XML_PATH MODULE_NAME PARSER_TYPE)
  # Figure out which parser to use when looking for various dependencies.
  set(PARSER_PY ${FPRIME_CORE_DIR}/cmake/parser/${PARSER_TYPE}_xml.py)
  if (${PARSER_TYPE} STREQUAL "topology")
    execute_process(
      COMMAND ${FPRIME_CORE_DIR}/cmake/parser/topology_xml.py "--targets" "${XML_PATH}"
      RESULT_VARIABLE ERR_RETURN
      OUTPUT_VARIABLE TARGETS
    )
  else() 
    execute_process(
      COMMAND ${PARSER_PY} "${XML_PATH}" "${MODULE_NAME}"
      RESULT_VARIABLE ERR_RETURN
      OUTPUT_VARIABLE TARGETS
    )
  endif()
  # Check parser return code
  if(ERR_RETURN)
     message(FATAL_ERROR "Failed to parse ${XML_PATH} using parser ${PARSER_PY} with result: ${ERR_RETURN}")
  endif()
  # For every dected dependency, add them to the supplied module. This enforces build order.
  # Also set the link dependencies on this module. CMake rolls-up link dependencies, and thus
  # this prevents the need for manually specifying link orders.
  foreach(TARGET ${TARGETS})
    add_dependencies(${MODULE_NAME} ${TARGET})
    target_link_libraries(${MODULE_NAME} ${TARGET})
  endforeach()
endfunction(fprime_dependencies)
