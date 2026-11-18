
# Targets

The build system supplies the ability to register extra targets that allow the user to register new
functionality to the build system. For example, the user may wish to create a build target that
counts the total number of files in the system.  This would be the place to register such a target
and have it available to the build system.

Targets are applied both at the global scope and per-module scope. Thus each target can provide a set of build targets (one per registered module) and a global build target. For example, a counting target might provide a `count` global target to count all files, and a `<MODULE>_count` to count the files of a given module.

## Built-In Targets

The CMake system supplies several targets that are useful for all projects and thus are included
as part of the CMake system. These targets are defined in [cmake/target](https://github.com/nasa/fprime/tree/devel/cmake/target).

### `all` and `universe`

Two targets come from CMake and the module registration itself rather than from `cmake/target`:

CMake's `all` target builds the project: the modules, deployments, and unit tests registered by the project, plus the
framework and library modules they depend upon. Framework and library modules the project does not use are registered
`EXCLUDE_FROM_ALL` (option `FPRIME_EXCLUDE_LIBRARIES_FROM_ALL`, default `ON`) and are built only when named directly or
depended upon. The `universe` target builds every registered module, excluded or not:

```bash
cmake --build . --target all        # the project and what it depends upon (fprime-util build at the project root)
cmake --build . --target universe   # everything the build system knows about
```

In unit-test builds (`BUILD_TESTING=ON`) every test carries the CTest label `fprime_universe`, and the tests built by
`all` also carry `fprime_project`. `ctest -L fprime_project` runs the project's tests only and `ctest -L fprime_universe`
runs every test. The label names are recorded in the CMake cache as `FPRIME_PROJECT_TEST_LABEL` and
`FPRIME_UNIVERSE_TEST_LABEL`. fprime-tools releases after 4.4.0 run by these labels: `fprime-util check` at the project
root and `fprime-util check --project` (from any directory of the project) run the `fprime_project` tests, and
`fprime-util check --all` runs the `fprime_universe` tests. The tests built by `all` are also listed by name in the
root `tests.fprime-util`, which earlier fprime-util versions use to select the same tests with a `ctest -R` expression.

A module may exclude itself regardless of this option with the `EXCLUDE_FROM_ALL` argument of its `register_fprime_*`
call (or of `add_fprime_subdirectory`). `EXCLUDE_FROM_ALL` has its CMake meaning however it was set: the module is left
out of `all`, builds when named or depended upon, and is part of `universe`.

When F Prime itself is the top-level CMake project (building the framework), the option is forced `OFF` so that `all`
builds everything. Projects restore that behavior with `FPRIME_EXCLUDE_LIBRARIES_FROM_ALL=OFF`
(`default_cmake_options` in `settings.ini`, a `set()` before `find_package(FPrime)`/`add_subdirectory`, or `-D`).


## Adding Custom Targets

See the [Customization Guide](cmake-customization.md) for a description of adding custom targets.
