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
# Function `dictionary_installable_files`:
#
# Selects, from the dictionary autocoder outputs of a module, the files that are installed to `dict/` and recorded in
# the `FPRIME_DICTIONARIES` property: only the `<Deployment>TopologyDictionary.json` files. fpp-to-dict also writes
# `<System>SystemDictionary.json`; it stays a build output of the dictionary step but is not installed, since
# fprime-gds and other tools locate the dictionary by globbing `dict/*Dictionary.json` and refuse several matches.
#
# - **DICTIONARY_OUTPUTS:** list of files produced by the dictionary autocoder
# - **OUTPUT_VAR:** variable to set with the installable subset
####
function(dictionary_installable_files DICTIONARY_OUTPUTS OUTPUT_VAR)
    set(INSTALLABLE "${DICTIONARY_OUTPUTS}")
    list(FILTER INSTALLABLE INCLUDE REGEX ".*TopologyDictionary\\.json$")
    set(${OUTPUT_VAR} "${INSTALLABLE}" PARENT_SCOPE)
endfunction(dictionary_installable_files)

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
    list(REMOVE_ITEM DEPENDENCIES "${MODULE}")

    # Collect install commands and the files they install so a single OUTPUT-tracked command can run them
    set(INSTALL_COMMANDS)
    set(INSTALL_INPUTS)
    set(DICTIONARY_TARGETS)
    dictionary_installable_files("${AUTOCODER_GENERATED_OTHER}" INSTALLABLE_DICTIONARIES)
    if(INSTALLABLE_DICTIONARIES)
        install(FILES ${INSTALLABLE_DICTIONARIES} DESTINATION ${TOOLCHAIN_NAME}/${MODULE}/dict COMPONENT "${MODULE}_${TARGET}")
        list(APPEND INSTALL_COMMANDS COMMAND "${CMAKE_COMMAND}"
            -DCMAKE_INSTALL_COMPONENT=${MODULE}_${TARGET}
            -DFPRIME_INSTALL_DEST=${FPRIME_INSTALL_DEST}
            -DFPRIME_BUILD_DIR=${CMAKE_BINARY_DIR}
            -P ${FPRIME_FRAMEWORK_PATH}/cmake/target/fprime_install.cmake)
        list(APPEND INSTALL_INPUTS ${INSTALLABLE_DICTIONARIES})
    endif()

    # Loop through all recursive dependencies and find dictionary targets
    foreach(DEPENDENCY IN LISTS DEPENDENCIES)
        if (TARGET "${DEPENDENCY}_${TARGET}")
            get_target_property(DICTIONARY_FILES "${DEPENDENCY}" FPRIME_DICTIONARIES)
            fprime_cmake_ASSERT("No dictionary files defined for ${DEPENDENCY}" DICTIONARY_FILES)
            install(FILES ${DICTIONARY_FILES} DESTINATION ${TOOLCHAIN_NAME}/${MODULE}/dict COMPONENT "${MODULE}_${DEPENDENCY}_${TARGET}")
            list(APPEND INSTALL_COMMANDS COMMAND "${CMAKE_COMMAND}"
                -DCMAKE_INSTALL_COMPONENT=${MODULE}_${DEPENDENCY}_${TARGET}
                -DFPRIME_INSTALL_DEST=${FPRIME_INSTALL_DEST}
                -DFPRIME_BUILD_DIR=${CMAKE_BINARY_DIR}
                -P ${FPRIME_FRAMEWORK_PATH}/cmake/target/fprime_install.cmake)
            list(APPEND INSTALL_INPUTS ${DICTIONARY_FILES})
            list(APPEND DICTIONARY_TARGETS "${DEPENDENCY}_${TARGET}")
        endif()
    endforeach()

    if (INSTALL_COMMANDS)
        # The stamp is a real OUTPUT, so Ninja/Make skip the install when no dictionary input changed
        set(INSTALL_STAMP "${CMAKE_CURRENT_BINARY_DIR}/${MODULE}_${TARGET}.installed")
        add_custom_command(OUTPUT "${INSTALL_STAMP}"
            ${INSTALL_COMMANDS}
            COMMAND "${CMAKE_COMMAND}" -E touch "${INSTALL_STAMP}"
            DEPENDS ${INSTALL_INPUTS}
            COMMENT "Installing dictionaries for ${MODULE}"
        )
        add_custom_target("${MODULE}_${TARGET}" DEPENDS "${INSTALL_STAMP}")
    else()
        add_custom_target("${MODULE}_${TARGET}" DEPENDS ${AUTOCODER_GENERATED_OTHER})
    endif()
    # Make deployment depend on the module dictionary targets
    foreach(DICTIONARY_TARGET IN LISTS DICTIONARY_TARGETS)
        add_dependencies("${MODULE}_${TARGET}" "${DICTIONARY_TARGET}")
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
        dictionary_installable_files("${AUTOCODER_GENERATED_OTHER}" INSTALLABLE_DICTIONARIES)
        append_list_property("${INSTALLABLE_DICTIONARIES}" TARGET "${MODULE_NAME}" PROPERTY FPRIME_DICTIONARIES)
        add_custom_target("${MODULE_NAME}_${TARGET_NAME}" DEPENDS ${AUTOCODER_GENERATED_OTHER})
    endif()
endfunction(dictionary_add_module_target)
