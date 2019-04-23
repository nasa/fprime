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
# Function `generic_autocoder`:
#
# This function controls the the generation of the auto-coded files, generically, for serializables, ports,
# and components. It then mines the XML files for dependencies and then adds them as dependencies to the 
# module being built.
#
# - **MODULE_NAME:** name of the module which is being auto-coded.
# - **AUTOCODER_INPUT_FILES:** list of input files sent to the autocoder
# - **AC_TYPE:** type of the auto-coder being invoked,
#
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
      get_filename_component(INPUT_FILE_REAL "${INPUT_FILE}" REALPATH)
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
      acwrap("${AC_TYPE}" "${AC_FINAL_SOURCE}" "${AC_FINAL_HEADER}"  "${INPUT_FILE_REAL}")

      # Generated and detected dependencies
      add_generated_sources(${AC_FINAL_SOURCE} ${AC_FINAL_HEADER} ${MODULE_NAME})
      fprime_dependencies(${INPUT_FILE_REAL} ${MODULE_NAME} ${LOWER_TYPE})

      # Pass list of autocoder outputs out of the module
      set(AC_OUTPUTS "${AC_OUTPUTS}" PARENT_SCOPE)
    endif()
  endforeach()
endfunction(generic_autocoder)

# TODO: enumerations don't work super well with the system.  Don't use.
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
# Function `generate_module`:
#
# Generates the module as an F prime module. This means that it will process autocoding,
# and dependencies. Hard sources are not added here, but in the caller. This will all be
# generated into a library.
#
# - **OBJ_NAME:** object name to add dependencies to. 
# - **AUTOCODER_INPUT_FILES:** files to pass to the autocoder
# - **SOURCE_FILES:** source file inputs
# - **LINK_DEPS:** link-time dependecies like -lm or -lpthread
# - **MOD_DEPS:** CMake module dependencies
#
####
function(generate_module OBJ_NAME AUTOCODER_INPUT_FILES SOURCE_FILES LINK_DEPS MOD_DEPS)
  # If there are  build flags, set them now 
  if (DEFINED BUILD_FLAGS)
      target_compile_definitions(${OBJ_NAME} PUBLIC ${BUILD_FLAGS})
  endif()
  # Add dependencies on autocoder
  add_dependencies(${OBJ_NAME} ${CODEGEN_TARGET})

  # Go through each auto-coder type and process every file in the AC list. Each item of each type
  # will generate autocodor outputs.
  generic_autocoder(${OBJ_NAME} "${AUTOCODER_INPUT_FILES}" "enum")
  generic_autocoder(${OBJ_NAME} "${AUTOCODER_INPUT_FILES}" "serializable")
  generic_autocoder(${OBJ_NAME} "${AUTOCODER_INPUT_FILES}" "port")
  generic_autocoder(${OBJ_NAME} "${AUTOCODER_INPUT_FILES}" "component")
  generic_autocoder(${OBJ_NAME} "${AUTOCODER_INPUT_FILES}" "topology")

  # Add in all non-module link (-l) dependencies
  target_link_libraries(${OBJ_NAME} ${LINK_DEPS})

  # Add in specified (non-detected) mod dependencies, and Dict dependencies therein.
  foreach(MOD_DEP ${MOD_DEPS})
      get_module_name(${MOD_DEP})
      add_dependencies(${OBJ_NAME} ${MODULE_NAME})
      target_link_libraries(${OBJ_NAME} ${MODULE_NAME})
      add_dict_deps(${OBJ_NAME} ${MODULE_NAME})
  endforeach()
  
  # Remove empty source from target
  get_target_property(FINAL_SOURCE_FILES ${OBJ_NAME} SOURCES)
  list(REMOVE_ITEM FINAL_SOURCE_FILES ${EMPTY_C_SRC})
  set_target_properties(
     ${OBJ_NAME}
     PROPERTIES
     SOURCES "${FINAL_SOURCE_FILES}"
  )

  # Register extra targets at the very end, once all of the core functions are properly setup.
  setup_all_module_targets(${OBJ_NAME} "${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}" "${AC_OUTPUTS}")
endfunction(generate_module)
####
# Function `generate_library`:
#
# Generates a library as part of F prime. This runs the AC and all the other items for the build.
# It takes SOURCE_FILES_INPUT and DEPS_INPUT, splits them up into ac sources, sources, mod deps,
# and library deps.
#
# - *SOURCE_FILES_INPUT:* source files that will be split into AC and normal sources.
# - *DEPS_INPUT:* dependencies bound for link and cmake dependencies
#
####
function(generate_library SOURCE_FILES_INPUT DEPS_INPUT)
  # Set the following variables from the existing SOURCE_FILES and LINK_DEPS by splitting them into
  # their separate peices. 
  #
  # AUTOCODER_INPUT_FILES = *.xml and *.txt in SOURCE_FILES_INPUT, fed to auto-coder
  # SOURCE_FILES = all other items in SOURCE_FILES_INPUT, set as compile-time sources
  # LINK_DEPS = -l link flags given to DEPS_INPUT
  # MOD_DEPS = All other module inputs DEPS_INPUT
  split_source_files("${SOURCE_FILES_INPUT}")
  split_dependencies("${DEPS_INPUT}")

  # Sets MODULE_NAME to unique name based on path, and then adds the library of
  get_module_name(${CMAKE_CURRENT_LIST_DIR})
  message(STATUS "Adding library: ${MODULE_NAME}")
  # Add the library name
  add_library(
    ${MODULE_NAME}
    ${FPRIME_LIB_TYPE}
    ${SOURCE_FILES}
    ${EMPTY_C_SRC} # Added to suppress warning if module only has autocode
  )
  generate_module(${MODULE_NAME} "${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}" "${LINK_DEPS}" "${MOD_DEPS}")
  # Install the executable
  install(TARGETS "${MODULE_NAME}"
        RUNTIME DESTINATION "bin/${PLATFORM}"
        LIBRARY DESTINATION "lib/${PLATFORM}"
        ARCHIVE DESTINATION "lib/static/${PLATFORM}"
        OPTIONAL)
  # Link library list output on per-module basis
  if (CMAKE_DEBUG_OUTPUT)
	  print_dependencies(${MODULE_NAME})
  endif()
endfunction(generate_library)
