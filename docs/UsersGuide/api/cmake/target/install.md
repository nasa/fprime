**Note:** auto-generated from comments in: ./target/install.cmake

## target/install.cmake:

Installs fprime into the build-artifacts folder. This is done using CMake's install command. Requires CMake 3.13+.


## function(install_add_deployment_target MODULE TARGET SOURCES DEPENDENCIES FULL_DEPENDENCIES)
    set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)
    install(TARGETS ${MODULE} ${FULL_DEPENDENCIES}
            RUNTIME DESTINATION ${TOOLCHAIN_NAME}/bin
            LIBRARY DESTINATION ${TOOLCHAIN_NAME}/lib
            ARCHIVE DESTINATION ${TOOLCHAIN_NAME}/lib/static)
    get_property(DICTIONARY GLOBAL PROPERTY DICTIONARY_FILE)
    install(FILES ${DICTIONARY} DESTINATION ${TOOLCHAIN_NAME}/dict)
    add_custom_command(TARGET "${MODULE}" POST_BUILD COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target install)
endfunction()

Install is per-deployment, a module-by-module variant does not make sense
function(install_add_module_target MODULE_NAME TARGET_NAME SOURCE_FILES DEPENDENCIES)
endfunction(install_add_module_target)
