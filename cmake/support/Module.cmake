####
# Module.cmake:
#
# This cmake file contains the functions needed to compile a module for F prime. This
# includes code for generating Enums, Serializables, Ports, Components, and Topologies.
#
# These are used as the building blocks of F prime items. This includes deployments,
# tools, and indiviual components.
####
# Include some helper libraries
include("${CMAKE_CURRENT_LIST_DIR}/Utils.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/AC_Utils.cmake")

####
# Autocoder:
#
# This function controls the the generation of the auto-coded filesi, generically, for serializables, ports,
# and components. It then mines the XML files for dependencies and then adds them as dependencies to the 
# module being built.
#
# \param MODULE_NAME: name of the module which is being auto-coded.
# \param AUTOCODER_INPUT_FILES: list of input files sent to the autocoder
# \param AC_TYPE: type of the auto-coder being invoked,
####
function(generic_autocoder MODULE_NAME AUTOCODER_INPUT_FILES AC_TYPE)
  # Setup needed variants of AC_TYPE in order to power this function.
  # We need the following variants:
  #    - Lowercase (as passed in but forced here)
  #    - Pascal case (title case) first letter capitalized
  # Note: AC_LETTER, AC_UP_LETTER, AC_REMAINDER are temporary variables
  string(TOLOWER ${AC_TYPE} LOWER_TYPE)
  string(SUBSTRING ${LOWER_TYPE} 0 1 AC_LETTER)
  string(TOUPPER ${AC_LETTER} AC_UP_LETTER)
  string(SUBSTRING ${LOWER_TYPE} 1 -1 AC_REMAINDER)
  string(CONCAT NAME_TYPE ${AC_UP_LETTER} ${AC_REMAINDER})
  # Topology requires the "App" token in the name
  if(AC_TYPE STREQUAL "topology")
      string(CONCAT NAME_TYPE ${NAME_TYPE} "App")
  endif()
  # Go through every auto-coder file and then check to see if it is of this type.  If it matches this
  # type, then we continue. Otherwise, this is skipped as we are not generating this type.
  foreach(INPUT_FILE ${AUTOCODER_INPUT_FILES})
    # Check to see if the auto-coder input file is a serializable XML or if it something else
    # Als grab its name as it will be needed later
    string(REGEX MATCH "([a-zA-Z0-9\-_]+)${NAME_TYPE}Ai.xml" AC_XML "${INPUT_FILE}")
    string(REGEX REPLACE "([a-zA-Z0-9\-_]+)(${NAME_TYPE}Ai.xml)" "\\1" AC_NAME "${AC_XML}")
    if(NOT ${AC_XML} STREQUAL "")
      message(STATUS "\tFound ${LOWER_TYPE}: ${AC_NAME} from ${AC_XML}")
      # The build system intrinsically depends on these AC_XML files, so add it to the CMAKE_CONFIGURE_DEPENDS  
      set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${AC_XML})

      # Calculate the Ac .hpp and .cpp files that should be generated
      string(CONCAT AC_HEADER ${AC_NAME} "${NAME_TYPE}Ac.hpp")
      string(CONCAT AC_SOURCE ${AC_NAME} "${NAME_TYPE}Ac.cpp")
      # AC files may be considered source files, or they may be considered build artifacts. This is set via
      # cmake configuration and controls the location of the output.
      if (GENERATE_AC_IN_SOURCE)
          set(AC_FINAL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
      else()
          set(AC_FINAL_DIR ${CMAKE_CURRENT_BINARY_DIR})
      endif()
      string(CONCAT AC_FINAL_HEADER ${AC_FINAL_DIR} "/" ${AC_HEADER})
      string(CONCAT AC_FINAL_SOURCE ${AC_FINAL_DIR} "/" ${AC_SOURCE})
      string(CONCAT AC_FINAL_XML ${CMAKE_CURRENT_LIST_DIR} "/" ${AC_XML})
      acwrap("${AC_TYPE}" "${AC_FINAL_SOURCE}" "${AC_FINAL_HEADER}"  "${AC_FINAL_XML}")

      # Serializables and topologies generate dictionaries
      if (${AC_TYPE} STREQUAL "topology" OR ${AC_TYPE} STREQUAL "serializable")
          setup_module_dicts(${MODULE_NAME} ${AC_XML} ${AC_OUTPUTS})
      endif()
      # Generated and detected dependencies
      add_generated_sources(${AC_FINAL_SOURCE} ${AC_FINAL_HEADER} ${MODULE_NAME})
      fprime_dependencies(${AC_FINAL_XML} ${MODULE_NAME} ${LOWER_TYPE})
    endif()
  endforeach()
endfunction(generic_autocoder)

# Function for invokeing enum autocoder: TODO fix enumerations
function(enum_autocoder MODULE_NAME AUTOCODER_INPUT_FILES)
  # Search for enum txt files
  foreach(INPUT_FILE ${AUTOCODER_INPUT_FILES})
    string(REGEX MATCH "([a-zA-Z0-9\-_]+)EnumAi.txt" ENUM_TXT "${INPUT_FILE}")
    if(NOT ${ENUM_TXT} STREQUAL "")
      # Extract enum name
      string(REGEX REPLACE "([a-zA-Z0-9\-_]+)(EnumAi.txt)" "\\1" ENUM_NAME "${ENUM_TXT}")
      message(STATUS "\tFound enum: ${ENUM_NAME}")

      # Add enum header and source
      string(CONCAT ENUM_HEADER ${ENUM_NAME} "EnumAc.hpp")
      string(CONCAT ENUM_SOURCE ${ENUM_NAME} "EnumAc.cpp")
      string(CONCAT ENUM_PY ${ENUM_NAME} ".py")
      # AC files may be considered source files, or they may be considered build artifacts. This is set via
      # cmake configuration and controls the location of the output.
      if (GENERATE_AC_IN_SOURCE)
          set(ENUM_FINAL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
      else()
          set(ENUM_FINAL_DIR ${CMAKE_CURRENT_BINARY_DIR})
      endif()
      # Invoke autocoder to produce enum header
      add_custom_command(
        OUTPUT ${ENUM_FINAL_DIR}/${ENUM_HEADER} ${ENUM_FINAL_DIR}/${ENUM_SOURCE}
        COMMAND ${CMAKE_COMMAND} -E env SHELL_AUTOCODER_DIR=${SHELL_AUTOCODER_DIR}
        ${FPRIME_CORE_DIR}/cmake/wrapper/enumgen.sh ${CMAKE_CURRENT_SOURCE_DIR}/${ENUM_TXT} ${ENUM_FINAL_DIR}
        DEPENDS ${ENUM_TXT}
      )

      # Add autocode to module
      target_sources(
        ${MODULE_NAME}
        PRIVATE ${ENUM_FINAL_DIR}/${ENUM_SOURCE} ${ENUM_FINAL_DIR}/${ENUM_HEADER}
      )
    endif()
  endforeach()
endfunction(enum_autocoder)

####
# Serializable/port/component/topology autocoders:
#
# This function controls the the generation of the auto-coded files for serializable/port/component/topology
# objects in the system. It then mines the XML files for dependencies and then adds them as dependencies to
#the module being built.
#
# These all defer to a central module
####
function(serializable_autocoder MODULE_NAME AUTOCODER_INPUT_FILES)
  generic_autocoder(${MODULE_NAME} "${AUTOCODER_INPUT_FILES}" "serializable")
endfunction(serializable_autocoder)

# Function for invokeing port autocoder
function(port_autocoder MODULE_NAME AUTOCODER_INPUT_FILES)
  generic_autocoder(${MODULE_NAME} "${AUTOCODER_INPUT_FILES}" "port")
endfunction(port_autocoder)

# Function for invokeing component autocoder
function(component_autocoder MODULE_NAME AUTOCODER_INPUT_FILES)
  generic_autocoder(${MODULE_NAME} "${AUTOCODER_INPUT_FILES}" "component")
endfunction(component_autocoder)

# Function for invokeing topology autocoder
function(topology_autocoder MODULE_NAME AUTOCODER_INPUT_FILES)
  generic_autocoder(${MODULE_NAME} "${AUTOCODER_INPUT_FILES}" "topology")
endfunction(topology_autocoder)

####
# Generate Module:
#
# Generates modules for use in the CMake system. This process all types of
# modules.  It adds a library for linking of the module, and sets up all of
# the dependencies.
####
function(generate_module AUTOCODER_INPUT_FILES SOURCE_FILES LINK_DEPS)
  # Sets MODULE_NAME to unique name based on path, and then adds the library of
  get_module_name(${CMAKE_CURRENT_LIST_DIR})
  add_library(
    ${MODULE_NAME}
    ${FPRIME_LIB_TYPE}
    ${SOURCE_FILES}
    ${EMPTY_C_SRC} # Added to suppress warning if module only has autocode
  )
  message(STATUS "Adding library: ${MODULE_NAME}")
  # Add dependencies on autocoder
  add_dependencies(${MODULE_NAME} ${CODEGEN_TARGET})

  # Go through each auto-coder type and process every file in the AC list. Each item of each type
  # will generate autocodor outputs.
  enum_autocoder(${MODULE_NAME} "${AUTOCODER_INPUT_FILES}")
  serializable_autocoder(${MODULE_NAME} "${AUTOCODER_INPUT_FILES}")
  port_autocoder(${MODULE_NAME} "${AUTOCODER_INPUT_FILES}")
  component_autocoder(${MODULE_NAME} "${AUTOCODER_INPUT_FILES}")
  topology_autocoder(${MODULE_NAME} "${AUTOCODER_INPUT_FILES}")

  # Add in specified (non-detected) link dependencies, and Dict dependencies therein.
  set(OLD_MODULE_NAME ${MODULE_NAME})
  foreach(LINK_DEP ${LINK_DEPS})
    if ("${LINK_DEP}" MATCHES "-l.*")
      target_link_libraries("${OLD_MODULE_NAME}" "${LINK_DEP}")
    else()
      get_module_name(${LINK_DEP})
      add_dependencies(${OLD_MODULE_NAME}  ${MODULE_NAME})
      target_link_libraries(${OLD_MODULE_NAME}  ${MODULE_NAME})
      add_dict_deps(${OLD_MODULE_NAME}  ${MODULE_NAME})
    endif()
  endforeach()
  set(MODULE_NAME ${OLD_MODULE_NAME})

  # Remove empty source from target
  get_target_property(FINAL_SOURCE_FILES ${MODULE_NAME} SOURCES)
  list(REMOVE_ITEM FINAL_SOURCE_FILES ${EMPTY_C_SRC})
  set_target_properties(
     ${MODULE_NAME}
     PROPERTIES
     SOURCES "${FINAL_SOURCE_FILES}"
  )
  # Link library list output on per-module basis
  if (CMAKE_DEBUG_OUTPUT)
     get_target_property(OUT "${MODULE_NAME}" LINK_LIBRARIES)
     if (OUT MATCHES ".*-NOTFOUND")
       set(OUT "--none--")
     endif()
     message(STATUS "\tLinks dependencies: ${OUT}")
  endif()
  # Create unit test module
  generate_ut_library(${MODULE_NAME} "${FINAL_SOURCE_FILES}")
endfunction(generate_module)
