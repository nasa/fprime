# Integrating New Targets

In F´, targets are used to trigger various parts of the system to run. In v3.0.0, everything is a target. The build is
a target, dictionaries are another target, etc. This guide will walk you through integrating a new target and the most
common case: calling a new autocoder from that target.

## Target CMake Structure

F´ targets have the option to run at any or all of the three following levels in the CMake setup: modules, deployments, and
globally. Module targets run on every module defining the `<target>_register_fprime_module` functions. Deployment
targets run on deployments defined with the `<target>_register_fprime_deployment` calls, and global targets run once.

Targets are CMake files that define three functions, one for each level, and are described below. The filename of this
CMake without the `.cmake` extension determines the`<target>` name. e.g. `utility.cmake` will define the `utility`
target.

Targets stages are defined in the following order:

1. Global targets are setup
2. Pre-Module targets are setup
3. Deployment targets are setup

This means that global targets are typically created without dependencies, and each module-level target will add itself
as a dependency of the global target. Deployment targets are provided a full list of dependencies and thus can be set up
with targets. This is done because the dependency tree cannot be known until after each module is set up.

## Function `<target>_add_global_target`

This function must add a global target to be run.  The target name is provided for convenience. Most of the time this is
used to register a top-level target that will act as a trigger for all module-level targets.

This example adds an empty global target. We will attach module level targets during the `<target>_add_module_target`
call.

```cmake
function(utility_add_global_target TARGET)
    add_custom_target(${TARGET})
endfunction(utility_add_global_target)
```

## Function `<target>_add_module_target`

This function adds a target per module. It is supplied the module, target, a list of source files supplied to the
module, and a list of dependencies supplied to the module (i.e. the `MOD_DEPS` variable).  Most commonly, this is done
to run autocoders on the module. The name of the target added should be: `${TARGET}_${MODULE}`.

Module targets often attach to the global target as dependencies such that all the module targets run before the global
target runs. An example is provided that runs `my_autocoder` on each module.

```cmake
include(autocoder/autocoder)
function(add_module_target MODULE TARGET SOURCE_FILES DEPENDENCIES)
    # Run the autocoder setup
    run_ac_set("${SOURCE_FILES}" autocoder/my-autocoder)
    # Add the module target, and use DEPENDS to trigger generation of autocoder outputs
    add_custom_target(${TARGET}_${MODULE} DEPENDS ${AC_GENERATED})
    # Attach dependencies to global target
    add_dependencies(${TARGET} ${TARGET}_${MODULE}) # Attach to the global target
endfunction(add_module_target)
```

## Function `add_deployment_target`

This function must add a deployment level target. The target name, module or deployment name, module dependencies, and
the full dependency list are supplied. This function can be used to add targets to a deployment where it is helpful to
know all the needed dependencies. e.g. the install target is registered on a deployment and installs both the deployment
executable and all the archive/shared libraries.  The name of the target added should be: `${TARGET}_${MODULE}`

This example registers the deployment to the global target and depends on `FULL_DEPENDENCIES` to ensure each dependent
module level target is run.

```cmake
function(add_deployment_target MODULE TARGET SOURCES DEPENDENCIES FULL_DEPENDENCIES)
    add_custom_target(${TARGET}_${MODULE} DEPENDS ${FULL_DEPENDENCIES})
    add_dependencies(${TARGET} ${TARGET}_${MODULE})
endfunction(add_deployment_target)
```