####
# default.cmake:
#
# Defaults for target implementations. This effectively "clears" the definitions, and thus allows a default behavior
# after another target was invoked.
####

####
# `add_global_target`:
#
# The default implementation defines the target using `add_custom_target` and nothing more.
####
function(add_global_target TARGET)
    if (CMAKE_DEBUG_OUTPUT)
        message(STATUS "[target] Adding default global target: ${TARGET}")
    endif()
    add_custom_target(${TARGET})
endfunction(add_global_target)

####
# `add_deployment_target`:
#
# The default deployment target is a target which rolls-up all dependent targets through recursion.
# - **MODULE:** name of the deployment module. This is usually equivalent to $PROJECT_NAME.
# - **TARGET:** name of the top-target (e.g. dict). Use ${MODULE_NAME}_${TARGET_NAME} for a module specific target
# - **SOURCE:** list of source file inputs from the CMakeList.txt setup
# - **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
####
function(add_deployment_target MODULE TARGET SOURCES DIRECT_DEPENDENCIES FULL_DEPENDENCY_LIST)
    if (CMAKE_DEBUG_OUTPUT)
        message(STATUS "Adding default deployment target: ${MODULE}_${TARGET}")
    endif()
    add_custom_target("${MODULE}_${TARGET}")
    foreach(DEPENDENCY IN LISTS RESULTS)
        if (TARGET "${DEPENDENCY}_${TARGET}")
            add_dependencies("${MODULE}_${TARGET}" "${DEPENDENCY}_${TARGET}")
        endif()
    endforeach()
endfunction(add_deployment_target)

####
# `add_module_target`:
#
# Forces a fatal to ensure that every given target defines the add module target functionality. Implementors may supply
# a blank function, but not leave it undefined.
#
# - **MODULE:** name of the module
# - **TARGET:** name of the top-target (e.g. dict). Use ${MODULE_NAME}_${TARGET_NAME} for a module specific target
# - **SOURCE:** list of source file inputs from the CMakeList.txt setup
# - **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
####
function(add_module_target MODULE TARGET SOURCES DEPENDENCIES)
    if (CMAKE_DEBUG_OUTPUT)
        message(STATUS "Skipping module target: ${MODULE}_${TARGET}, default performs no action.")
    endif()
endfunction(add_module_target)
