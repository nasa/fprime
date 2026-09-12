# Working in `cmake/` — guide for AI agents

This directory implements the F Prime build system: it turns FPP models into
autocoded C++ and assembles modules, unit tests, and deployments. This file
routes you to the right file; the reference documentation is
[`docs/user-manual/build-system/`](../docs/user-manual/build-system/) — start
with `cmake-api.md` for the public API and `cmake-implementations.md` for the
internals.

## Public API — start here

`cmake/API.cmake` is the **only** external interface. Everything a project or
module `CMakeLists.txt` calls is defined there, documented inline above each
function:

| Function | Use |
| --- | --- |
| `register_fprime_module` | Register a module (component/port library) to receive autocoding. |
| `register_fprime_library` | Register a plain library that does not need autocoding. |
| `register_fprime_executable` | Register an executable. |
| `register_fprime_deployment` | Register a deployment (topology plus binary). |
| `register_fprime_ut` | Register a module's unit tests. |
| `register_fprime_config` | Register a configuration module. |
| `add_fprime_subdirectory` | Add a directory to the build (use instead of `add_subdirectory`). |
| `register_fprime_implementation`, `register_os_implementation` | Provide an implementation for an abstract interface (used heavily by `Os/`). |
| `register_fprime_target`, `register_fprime_ut_target`, `register_fprime_build_autocoder` | Add a custom build stage or autocoder. |

When changing the API, update its inline documentation and
`docs/user-manual/build-system/cmake-api.md` together.

## Where everything else lives

| Path | Contents |
| --- | --- |
| `API.cmake` | Public API (above). |
| `FPrime.cmake`, `FPrime-Code.cmake`, `FPrimeConfig.cmake` | Entry points that a project includes to set up the build. |
| `options.cmake` | All `FPRIME_*` build options and paths (`FPRIME_ENABLE_*`, sanitizers, `BUILD_TESTING`, framework and library locations). Look here before adding a new option. |
| `module.cmake`, `global_interface.cmake`, `flags.cmake`, `utilities.cmake` | Internals: how modules, interfaces, and compile flags are assembled. |
| `settings.cmake`, `settings/` | Parsing of a project's `settings.ini`. |
| `target/` | Build targets and stages: `build`, `ut`, `install`, `dictionary`, `sbom`, `version`, plus `sub-build/` and `tools/`. |
| `autocoder/` | FPP autocoder integration: `fpp.cmake`, `fpp_ut.cmake`, `autocoder.cmake`, and helper `scripts/`. |
| `platform/` | Platform definitions (`Linux.cmake`, `Darwin.cmake`, `unix/`) plus `platform.cmake.template` for a new platform. |
| `toolchain/` | Cross-compilation toolchains (`arm-*-linux`, `aarch64-*`, `raspberrypi`) plus `toolchain.cmake.template`. |
| `config_assembler.cmake` | Assembles the configuration headers a build uses. |
| `sanitizers.cmake` | Address/leak/UB/thread sanitizer wiring for unit tests. |
| `sub-build/` | The nested CMake invocations used for setup and utility work. |
| `test/` | Tests for the build system itself. |
| `docs/sdd.md` | Design document for the build system. |

## Conventions

- Adding a **platform** or **toolchain**: copy the corresponding `.template`
  file rather than an existing definition, and follow
  [`docs/user-manual/build-system/cmake-platforms.md`](../docs/user-manual/build-system/cmake-platforms.md)
  or `cmake-toolchains.md`.
- Adding an **option**: declare it in `options.cmake` with an `option()` /
  `set(... CACHE ...)` and document it in
  [`docs/user-manual/build-system/settings.md`](../docs/user-manual/build-system/settings.md)
  alongside the other `FPRIME_*` settings.
- Adding a **build stage**: implement it under `target/` and register it via
  `register_fprime_target`; see
  [`docs/user-manual/build-system/cmake-targets.md`](../docs/user-manual/build-system/cmake-targets.md).
- Use `skip_on_sub_build()` in code that must not run during a sub-build, and
  `restrict_platforms()` in modules that only build on some platforms.
- Autocoded output lives in the build cache (`build-fprime-*/`); it is a build
  product and is never edited or committed.
