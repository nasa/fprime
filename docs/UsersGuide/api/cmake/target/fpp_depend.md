**Note:** auto-generated from comments in: ./target/fpp_depend.cmake

## fpp_depend.cmake:

fpp_depend is a special target used to build cached information for fpp-depend output. It is run as part of the
sub-build that generates cached-information about the build itself.


## Function `fpp_depend_add_global_target`:

Sets up the `fpp_depend` target used to generate depend output across the whole build.
- **TARGET:** name of the target to setup (fpp_depend)


## Function `fpp_depend_add_deployment_target`:

Pass-through to fpp_depend_add_module_target. FULL_DEPENDENCIES is unused.


## Function `fpp_depend_add_module_target`:

Generates the cached fpp-depend output fore each module and registers the target to the global fpp_depend target.
- **MODULE:** module name, unused
- **TARGET:** name of the target to setup (fpp_depend)
- **SOURCES:** list of sources filtered to .fpp
- **DEPENDENCIES:** module dependencies, unused.


