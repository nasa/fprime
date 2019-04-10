####
# API.cmake:
#
# API of the F prime CMake system. An entry point accessed by users should be exposed here. Thus,
# a developer should understand these functions. Here-in, developers can find out how to do the
# following tasks:
#
#  - Register an F prime module (library).
#  - Register an F prime executable (deployment, other executable).
#  - Register an F prime unit-test (executable with special dependencies).
####

####
# register_fprime_module:
#
# Registers a module using the F prime build system. This comes with dependency management and F
# Prime autocoding capabilities. This requires two variables to define the autocoding and source
# inputs, and (optionally) any non-standard link dependencies.
#
# Required variables (defined in calling scope):
#
#   SOURCE_FILES - cmake list of input source files. Place any "*Ai.xml", "*.txt, "*.c", "*.cpp"
#                  etc. files here. This list will be split into autocoder inputs or sources.
#
#                  i.e. set(SOURCE_FILES
#                           MyComponentAi.xml
#                           SomeFile.cpp
#                           MyComponentImpl.cpp)
#
#   MOD_DEPS - (optional) cmake list of extra link dependencies. This is optional, and only needed
#              if non-standard link dependencies are used. If not set or supplied, only F prime
#              detectable dependencies will be available. Link flags like "-lpthread" can be here.
#
#              i.e. set(LINK_DEPS
#                       Module1
#                       Module2
#                       -lpthread)
#
# Note: if desired, these fields may be supplied in-order as arguments to the function. Passing
#       these as positional arguments overrides any specified in the parent scope.
#
#  TODO: EXAMPLES.
#
#
function(register_fprime_module)
    # SOURCE_FILES is supplied as the first positional -OR- as the list 'SOURCE_FILES'
    if (${ARGC} GREATER 0)
        set(SC_IFS "${ARGV0}")
    elseif(DEFINED SOURCE_FILES)
    	set(SC_IFS "${SOURCE_FILES}")
    else()
        message(FATAL_ERROR "'SOURCE_FILES' not defined in '${CMAKE_CURRENT_LIST_FILE}'.")
    endif()
    # MOD_DEPS is supplied as an optional second positional -OR- or as  the list 'MOD_DEPS'
    if (${ARGC} GREATER 1)
        set(MD_IFS "${ARGV1}")
    elseif(DEFINED MOD_DEPS)
    	set(MD_IFS "${MOD_DEPS}")
    elseif(${CMAKE_DEBUG_OUTPUT})
        message(STATUS "No exra 'MOD_DEPS' found in '${CMAKE_CURRENT_LIST_FILE}'.")
    endif()
    # Explicit call to module register
    generate_library("${SC_IFS}" "${MD_IFS}")
endfunction(register_fprime_module)

####
# register_fprime_executable:
#
# Registers an executable using the F prime build system. This comes with dependency management and
# F Prime autocoding capabilities. This requires three variables to define the executable name,
# autocoding and source inputs, and (optionally) any non-standard link dependencies.
#
# Required variables (defined in calling scope):
#
#   EXECUTABLE_NAME - (optional) executable name supplied. If not supplied, or passed in, then
#                     PROJECT_NAME from the CMake definitions is used.
#
#   SOURCE_FILES - cmake list of input source files. Place any "*Ai.xml", "*.txt, "*.c", "*.cpp"
#                  etc. files here. This list will be split into autocoder inputs or sources.
#
#                  i.e. set(SOURCE_FILES
#                           MyComponentAi.xml
#                           SomeFile.cpp
#                           MyComponentImpl.cpp)
#
#   MOD_DEPS - (optional) cmake list of extra link dependencies. This is optional, and only needed
#              if non-standard link dependencies are used. If not set or supplied, only F prime
#              detectable dependencies will be available. Link flags like "-lpthread" can be here.
#
#              i.e. set(LINK_DEPS
#                       Module1
#                       Module2
#                       -lpthread)
#
# Note: if desired, these fields may be supplied in-order as arguments to the function. Passing
#       these as positional arguments overrides any specified in the parent scope.
#
#  TODO: EXAMPLES.
#
#
function(register_fprime_executable)
    # PROJECT_NAME is used for the executable name, unless otherwise specified.
    if (${ARGC} GREATER 0)
        set(EX_NAME "${ARGV0}")
    elseif(DEFINED EXECUTABLE_NAME)
        set(EX_NAME ${EXECUTABLE_NAME})
    elseif(DEFINED PROJECT_NAME)
    	set(EX_NAME "${PROJECT_NAME}")
    else()
        message(FATAL_ERROR "'EXECUTABLE_NAME' not defined in '${CMAKE_CURRENT_LIST_FILE}'.")
    endif()
    # SOURCE_FILES is supplied as the first positional -OR- as the list 'SOURCE_FILES'
    if (${ARGC} GREATER 1)
        set(SC_IFS "${ARGV1}")
    elseif(DEFINED SOURCE_FILES)
    	set(SC_IFS "${SOURCE_FILES}")
    else()
        message(FATAL_ERROR "'SOURCE_FILES' not defined in '${CMAKE_CURRENT_LIST_FILE}'.")
    endif()
    # MOD_DEPS is supplied as an optional second positional -OR- or as  the list 'MOD_DEPS'
    if (${ARGC} GREATER 2)
        set(MD_IFS "${ARGV2}")
    elseif(DEFINED MOD_DEPS)
    	set(MD_IFS "${MOD_DEPS}")
    elseif(${CMAKE_DEBUG_OUTPUT})
        message(STATUS "No extra 'MOD_DEPS' found in '${CMAKE_CURRENT_LIST_FILE}'.")
    endif()
    # Explicit call to module register
    generate_executable("${EX_NAME}" "${SC_IFS}" "${MD_IFS}")
endfunction(register_fprime_executable)


####
# register_fprime_ut:
#
# Registers an executable unit-test using the F prime build system. This comes with dependency
# management and F Prime autocoding capabilities. This requires five variables to define the unit
# test name, autocoding and source inputs for the unit test, and (optionally) any non-standard
# link dependencies.
#
# Note: This is ONLY run when the platform sets UT_BUILD to TRUE
#
# Required variables (defined in calling scope):
#
#   UT_NAME - (optional) executable name supplied. If not supplied, or passed in, then
#             the module name _exe will be used.
#
#   UT_SOURCE_FILES - cmake list of UT source files. Place any "*Ai.xml", "*.txt, "*.c", "*.cpp"
#                     etc. files here. This list will be split into autocoder inputs or sources.
#                     It is only used when building this specific unit test target.
#
#                     i.e. set(SOURCE_FILES
#                              MyComponentAi.xml
#                              SomeFile.cpp
#                              MyComponentImpl.cpp)
#
#   UT_MOD_DEPS - (optional) cmake list of extra link dependencies. This is optional, and only
#                 needed if non-standard link dependencies are used. If not set or supplied, only
#                 F prime detectable dependencies will be available. Link flags like "-lpthread"
#                 can be here.
#
#                 i.e. set(LINK_DEPS
#                          Module1
#                          Module2
#                          -lpthread)
#
#   Note: if desired, these fields may be supplied in-order as arguments to the function. Passing
#       these as positional arguments overrides any specified in the parent scope.
#
#   Note: UTs automaitcally depend on the module. In order to prevent this, explicitly pass in args
#         to this module, excluding the module.
#
#         e.g.
#             register_fprime_ut("MY_SPECIAL_UT" "${SOME_SOURCE_FILE_LIST}" "") #No dependenices.
#
#  TODO: EXAMPLES.
#
#
function(register_fprime_ut)
    #### CHECK UT BUILD ####
    if (NOT UT_BUILD)
        return()
    endif()
    get_module_name(${CMAKE_CURRENT_LIST_DIR})
    # PROJECT_NAME is used for the executable name, unless otherwise specified.
    if (${ARGC} GREATER 0)
        set(UT_NAME "${ARGV0}")
    elseif(DEFINED UT_NAME)
        set(UT_NAME ${UT_NAME})
    else()
    	set(UT_NAME "${MODULE_NAME}_exe")
    endif()
    # SOURCE_FILES is supplied as the first positional -OR- as the list 'SOURCE_FILES'
    if (${ARGC} GREATER 1)
        set(SC_IFS "${ARGV1}")
    elseif(DEFINED UT_SOURCE_FILES)
    	set(SC_IFS "${UT_SOURCE_FILES}")
    else()
        message(FATAL_ERROR "'UT_SOURCE_FILES' not defined in '${CMAKE_CURRENT_LIST_FILE}'.")
    endif()
    # MOD_DEPS is supplied as an optional second positional -OR- or as  the list 'MOD_DEPS'
    string(REGEX REPLACE "_${BUILD_SUFFIX}" "" MODULE_NAME_NO_SUFFIX "${MODULE_NAME}")
    if (${ARGC} GREATER 2)
        set(MD_IFS "${ARGV2}")
    elseif(DEFINED UT_MOD_DEPS)
    	set(MD_IFS "${MODULE_NAME_NO_SUFFIX};${UT_MOD_DEPS}")
    else()
        set(MD_IFS "${MODULE_NAME_NO_SUFFIX}")
        if (CMAKE_DEBUG_OUTPUT)
            message(STATUS "No exra 'MOD_DEPS' found in '${CMAKE_CURRENT_LIST_FILE}'.")
        endif()
    endif()
    # Explicit call to module register
    generate_ut("${UT_NAME}" "${SC_IFS}" "${MD_IFS}")
endfunction(register_fprime_ut)

