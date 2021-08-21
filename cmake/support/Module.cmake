####
# Module.cmake:
#
# This cmake file contains the functions needed to compile a module for F prime. This
# includes code for generating Enums, Serializables, Ports, Components, and Topologies.
#
# These are used as the building blocks of F prime items. This includes deployments,
# tools, and individual components.
####
# Include some helper libraries
include("${CMAKE_CURRENT_LIST_DIR}/Utils.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/AC_Utils.cmake")

####
# Function `generic_autocoder`:
#
# This function controls the the generation of the auto-coded files, generically, for serializables, ports,
# and components. It then mines the XML file for type and  dependencies and then adds them as dependencies to
# the module being built.
#
# - **MODULE_NAME:** name of the module which is being auto-coded.
# - **AUTOCODER_INPUT_FILES:** list of input files sent to the autocoder
# - **MOD_DEPS:** list of specified module dependencies
####
#function(generic_autocoder MODULE_NAME AUTOCODER_INPUT_FILES MOD_DEPS)
#  # Go through every auto-coder file and then run the autocoder detected by cracking open the XML file.
#  foreach(INPUT_FILE ${AUTOCODER_INPUT_FILES})
#      # Convert the input file into a real path to ensure the system knows what to work with
#      get_filename_component(INPUT_FILE "${INPUT_FILE}" REALPATH)
#
#      # Run the function required to get all information from the Ai file
#      fprime_ai_info("${INPUT_FILE}" "${MODULE_NAME}")
#      message(STATUS "\tFound ${XML_LOWER_TYPE}: ${AC_OBJ_NAME} in ${INPUT_FILE}")
#      # The build system intrinsically depends on these Ai.xmls and all files included by it
#      set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${INPUT_FILE};${FILE_DEPENDENCIES}")
#
#      # Calculate the full path to the Ac.hpp and Ac.cpp files that should be generated both the
#      string(CONCAT AC_FULL_HEADER ${CMAKE_CURRENT_BINARY_DIR} "/" "${AC_OBJ_NAME}" "${XML_TYPE}Ac.hpp")
#      string(CONCAT AC_FULL_SOURCE ${CMAKE_CURRENT_BINARY_DIR} "/" "${AC_OBJ_NAME}" "${XML_TYPE}Ac.cpp")
#
#      # Run the specific autocoder helper
#      acwrap("${XML_LOWER_TYPE}" "${AC_FULL_SOURCE}" "${AC_FULL_HEADER}"  "${INPUT_FILE}" "${FILE_DEPENDENCIES}" "${MOD_DEPS};${MODULE_DEPENDENCIES}")
#
#      add_generated_sources("${AC_FULL_SOURCE}" "${AC_FULL_HEADER}" "${MODULE_NAME}")
#      # For every detected dependency, add them to the supplied module. This enforces build order.
#      # Also set the link dependencies on this module. CMake rolls-up link dependencies, and thus
#      # this prevents the need for manually specifying link orders.
#      foreach(TARGET ${MODULE_DEPENDENCIES})
#          add_dependencies(${MODULE_NAME} "${TARGET}")
#          target_link_libraries(${MODULE_NAME} "${TARGET}")
#      endforeach()
#
#      # Pass list of autocoder outputs out of the module
#      set(AC_OUTPUTS "${AC_OUTPUTS}" PARENT_SCOPE)
#      set(MODULE_DEPENDENCIES "${MODULE_DEPENDENCIES}" PARENT_SCOPE)
#  endforeach()
#endfunction(generic_autocoder)


function(resolve_dependencies DEPENDENCIES AC_DEPENDENCIES OUTPUT_VAR)
    # Resolve all dependencies
    set(RESOLVED)
    foreach(DEPENDENCY IN LISTS DEPENDENCIES AC_DEPENDENCIES)
        get_module_name(${DEPENDENCY})
        # TODO:
        # TODO:
        # FIXME: EVIL
        if (MODULE_NAME STREQUAL "config" OR MODULE_NAME IN_LIST RESOLVED)
            continue()
        endif()
        # TODO
        # TODO:
        list(APPEND RESOLVED "${MODULE_NAME}")
    endforeach()
    set(${OUTPUT_VAR} "${RESOLVED}" PARENT_SCOPE)

endfunction(resolve_dependencies)

function(update_module MODULE_NAME SOURCES GENERATED EXCLUDED_SOURCES DEPENDENCIES)
    get_target_property(OUT "${MODULE_NAME}" LINK_LIBRARIES)
    message(STATUS "11111> ${OUT}")

    # For every detected dependency, add them to the supplied module. This enforces build order.
    # Also set the link dependencies on this module. CMake rolls-up link dependencies, and thus
    # this prevents the need for manually specifying link orders.
    foreach(DEPENDENCY ${DEPENDENCIES})
        if (NOT DEPENDENCY MATCHES "^-l.*")
            add_dependencies(${MODULE_NAME} "${DEPENDENCY}")
        endif()
        target_link_libraries(${MODULE_NAME} "${DEPENDENCY}")
    endforeach()
    # Compilable sources
    set(COMPILE_SOURCES)
    foreach(SOURCE IN LISTS SOURCES GENERATED)
        if (NOT SOURCE IN_LIST EXCLUDED_SOURCES)
            list(APPEND COMPILE_SOURCES "${SOURCE}")
        endif()
    endforeach()

    # Add auto-coded sources to the module as sources
    target_sources(${MODULE_NAME} PRIVATE ${COMPILE_SOURCES})
    # Set those files as generated to prevent build errors
    foreach(SOURCE IN LISTS GENERATED)
        set_source_files_properties(${SOURCE} PROPERTIES GENERATED TRUE)
    endforeach()
    # Includes the source, so that the Ac files can include source headers
    target_include_directories("${MODULE_NAME}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    get_target_property(OUT "${MODULE_NAME}" LINK_LIBRARIES)
    message(STATUS "22222> ${OUT}")
endfunction()



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
# - **LINK_DEPS:** link-time dependencies like -lm or -lpthread
# - **MOD_DEPS:** CMake module dependencies
####
function(generate_module OBJ_NAME SOURCES DEPENDENCIES)
  # If there are  build flags, set them now 
  if (DEFINED BUILD_FLAGS)
      target_compile_definitions(${OBJ_NAME} PUBLIC ${BUILD_FLAGS})
  endif()
  # Add dependencies on autocoder

  ac_process_sources("${SOURCES}")
  resolve_dependencies("${DEPENDENCIES}" "${AC_DEPENDENCIES}" RESOLVED)
  update_module("${OBJ_NAME}" "${SOURCES}" "${AC_GENERATED}" "${AC_SOURCES}" "${RESOLVED}")

  # Run autocoders for the set of identified Ai inputs
  #generic_autocoder(${OBJ_NAME} "${AUTOCODER_INPUT_FILES}" "${RESOLVED_DEPS}")


  # Add in all non-module link (-l) dependencies
  #target_link_libraries(${OBJ_NAME} ${LINK_DEPS})

  # Add in specified (non-detected) mod dependencies, and Dict dependencies therein.
  #foreach(MOD_DEP ${RESOLVED_DEPS})
  #    add_dependencies(${OBJ_NAME} ${MOD_DEP})
  #    target_link_libraries(${OBJ_NAME} ${MOD_DEP})
  #endforeach()

  # Remove empty source from target
  get_target_property(FINAL_SOURCE_FILES ${OBJ_NAME} SOURCES)
  list(REMOVE_ITEM FINAL_SOURCE_FILES ${EMPTY_C_SRC})
  set_target_properties(
     ${OBJ_NAME}
     PROPERTIES
     SOURCES "${FINAL_SOURCE_FILES}"
  )
  foreach(SRC_FILE ${FINAL_SOURCE_FILES})
      set_hash_flag("${SRC_FILE}")
  endforeach()


  # Register extra targets at the very end, once all of the core functions are properly setup.
  setup_all_module_targets(FPRIME_TARGET_LIST ${OBJ_NAME} "${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}" "${AC_OUTPUTS}" "${RESOLVED_DEPS}")

  if (CMAKE_DEBUG_OUTPUT)
      introspect("${OBJ_NAME}")
  endif()

  #set(AC_OUTPUTS "${AC_OUTPUTS}" PARENT_SCOPE)
endfunction(generate_module)

####
# Function `generate_library`:
#
# Generates a library as part of F prime. This runs the AC and all the other items for the build.
# It takes SOURCE_FILES_INPUT and DEPS_INPUT, splits them up into ac sources, sources, mod deps,
# and library deps.
# - *MODULE_NAME:* module name of library to build
# - *SOURCE_FILES_INPUT:* source files that will be split into AC and normal sources.
# - *DEPS_INPUT:* dependencies bound for link and cmake dependencies
#
####
function(generate_library MODULE_NAME SOURCE_FILES_INPUT DEPS_INPUT)
  # Set the following variables from the existing SOURCE_FILES and LINK_DEPS by splitting them into
  # their separate pieces. 
  #
  # AUTOCODER_INPUT_FILES = *.xml and *.txt in SOURCE_FILES_INPUT, fed to auto-coder
  # SOURCE_FILES = all other items in SOURCE_FILES_INPUT, set as compile-time sources
  # LINK_DEPS = -l link flags given to DEPS_INPUT
  # MOD_DEPS = All other module inputs DEPS_INPUT
  #split_source_files("${SOURCE_FILES_INPUT}")
  #split_dependencies("${DEPS_INPUT}")



  message(STATUS "Adding library: ${MODULE_NAME}")
  # Add the library name
  add_library(
    ${MODULE_NAME}
    ${EMPTY_C_SRC} # Added to suppress warning if module only has autocode
  )
  if (NOT DEFINED FPRIME_OBJECT_TYPE)
      set(FPRIME_OBJECT_TYPE "Library")
  endif()
  generate_module(${MODULE_NAME} "${SOURCE_FILES_INPUT}" "${DEPS_INPUT}")

  set(AC_OUTPUTS "${AC_OUTPUTS}" PARENT_SCOPE)
endfunction(generate_library)

