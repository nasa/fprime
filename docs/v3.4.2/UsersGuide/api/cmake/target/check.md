**Note:** auto-generated from comments in: ./target/check.cmake

## check.cmake:

Check target adds in an endpoint for running the individual unit tests.


##  Function `add_global_target`:

 Adds a global target. Note: only run for "BUILD_TESTING=ON" builds.

- **TARGET_NAME:** target name to be generated


## Function `add_deployment_target`:

Creates a target for "check" per-deployment, to run all UTs within that deployment.

- **MODULE:** name of the module
- **TARGET:** name of target to produce
- **SOURCES:** list of source file inputs
- **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
- **FULL_DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt


## Function `check_add_module_target`:

Creates each module's check targets. Note: only run for "BUILD_TESTING=ON" builds.

- **MODULE_NAME:** name of the module
- **TARGET_NAME:** name of target to produce
- **SOURCE_FILES:** list of source file inputs
- **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt


