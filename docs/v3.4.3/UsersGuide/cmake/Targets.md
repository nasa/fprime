
# Targets

The build system supplies the ability to register extra targets that allow the user to register new
functionality to the build system. For example, the user may wish to create a build target that
counts the total number of files in the system.  This would be the place to register such a target
and have it available to the build system.

Targets are applied both at the global scope and per-module scope. Thus each target can provide a set of build targets (one per registered module) and a global build target. For example, a counting target might provide a `count` global target to count all files, and a `<MODULE>_count` to count the files of a given module.

For projects generating GNU make files, these targets can be executed with the `make <target>` and
`make <MODULE>_<target>` commands. i.e. `make Svc_CmdDispatcher_coverage`.

## Built-In Targets

The CMake system supplies several targets that are useful for all projects and thus are included
as part of the CMake system. These targets are described in the [index](cmake-api.md)


## Adding Custom Targets

See the [Customization Guide](Customization.md) for a description of adding custom targets.
