####
# dictionary.cmake:
#
# Dictionary target that calls the FPP autocoder as part of the module function.
# The rest of the implementation is empty as requested.
####
include_guard()
include(autocoder/autocoder)

####
# Function `dictionary_add_global_target`:
#
# Adds a global dictionary target that depends on all module dictionary targets.
#
# - **TARGET_NAME:** target name to be generated
####
function(dictionary_add_global_target TARGET_NAME)
    add_custom_target(${TARGET_NAME})
endfunction(dictionary_add_global_target)

####
# Function `dictionary_add_deployment_target`:
#
# Creates a deployment-level dictionary target. Currently empty implementation.
#
# - **MODULE:** name of the module
# - **TARGET:** name of target to produce
# - **SOURCES:** list of source file inputs
# - **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
# - **FULL_DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
####
function(dictionary_add_deployment_target MODULE TARGET SOURCES DEPENDENCIES FULL_DEPENDENCIES)
    run_ac_set("${MODULE}" "autocoder/fpp")

    # Create deployment level target and remove the module from the list of dependencies
    add_custom_target("${MODULE}_${TARGET}" DEPENDS ${AUTOCODER_GENERATED_OTHER})
    list(REMOVE_ITEM DEPENDENCIES "${MODULE}")

    # Create a custom target with _dictionary suffix that depends on the generated files
    if(AUTOCODER_GENERATED_OTHER)
        # Install the files as a component. This is done here so it is output to the deployment directory
        install(FILES ${AUTOCODER_GENERATED_OTHER} DESTINATION ${TOOLCHAIN_NAME}/${MODULE}/dict COMPONENT "${MODULE}_${TARGET}")
        add_custom_command(TARGET "${MODULE}_${TARGET}" POST_BUILD COMMAND "${CMAKE_COMMAND}"
            -DCMAKE_INSTALL_COMPONENT=${MODULE}_${TARGET} -P ${CMAKE_BINARY_DIR}/cmake_install.cmake)
    endif()

    # Loop through all recursive dependencies and find dictionary targets
    foreach(DEPENDENCY IN LISTS DEPENDENCIES)
        if (TARGET "${DEPENDENCY}_${TARGET}")
            get_target_property(DICTIONARY_FILES "${DEPENDENCY}" FPRIME_DICTIONARIES)
            fprime_cmake_ASSERT("No dictionary files defined for ${DEPENDENCY}" DICTIONARY_FILES)
            # Install the files as a component. This is done here so it is output to the deployment directory
            install(FILES ${DICTIONARY_FILES} DESTINATION ${TOOLCHAIN_NAME}/${MODULE}/dict COMPONENT "${MODULE}_${DEPENDENCY}_${TARGET}")
            add_custom_command(TARGET "${MODULE}_${TARGET}" POST_BUILD COMMAND "${CMAKE_COMMAND}"
                -DCMAKE_INSTALL_COMPONENT=${MODULE}_${DEPENDENCY}_${TARGET} -P ${CMAKE_BINARY_DIR}/cmake_install.cmake)
            # Make deployment depend on the module dictionary target
            add_dependencies("${MODULE}_${TARGET}" "${DEPENDENCY}_${TARGET}")
        endif()
    endforeach()

    # Make the deployment and dictionary targets depend on the deployment dictionary target
    add_dependencies("${MODULE}" "${MODULE}_${TARGET}")
    add_dependencies(dictionary "${MODULE}_${TARGET}")
endfunction(dictionary_add_deployment_target)

####
# Function `dictionary_add_module_target`:
#
# Creates a module-level dictionary target that calls the FPP autocoder.
# The rest of the implementation is empty as requested.
#
# - **MODULE_NAME:** name of the module
# - **TARGET_NAME:** name of target to produce
# - **SOURCE_FILES:** list of source file inputs
# - **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
####
function(dictionary_add_module_target MODULE_NAME TARGET_NAME SOURCE_FILES DEPENDENCIES)
    run_ac_set("${MODULE_NAME}" "autocoder/fpp")
    
    # Create a custom target with _dictionary suffix that depends on the generated files
    if(AUTOCODER_GENERATED_OTHER)
        append_list_property("${AUTOCODER_GENERATED_OTHER}" TARGET "${MODULE_NAME}" PROPERTY FPRIME_DICTIONARIES)
        add_custom_target("${MODULE_NAME}_${TARGET_NAME}" DEPENDS ${AUTOCODER_GENERATED_OTHER})
    endif()
endfunction(dictionary_add_module_target)
