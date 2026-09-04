---
name: fprime-cmake-build-system
description: >
  Work with F Prime's custom CMake layer: the `register_fprime_*` API,
  build targets, toolchains and platform files, implementation
  selection, configuration modules, sub-builds, and `settings.ini`.
  Use when adding or wiring a module into a build, adding a custom
  build target, porting to a new platform or toolchain, choosing or
  defining an Os implementation, or diagnosing generate/build failures
  that are not compiler errors.
---

# Skill: F Prime CMake Build System

F Prime wraps CMake in an API layer so that modules declare sources and
autocoder inputs and get autocoding, dictionaries, unit tests, and
install rules for free. Canonical references:

- [Build system user manual](../../../docs/user-manual/build-system/01-cmake-intro.md)
- API and options reference — generated from the docblocks in
  [`API.cmake`](../../../cmake/API.cmake) and
  [`options.cmake`](../../../cmake/options.cmake); read those files
  directly, they are the source of truth
- [Build system SDD](../../../cmake/docs/sdd.md) (requirements, ops concepts)

Follow those for full syntax and examples; this skill adds orientation,
decision criteria, and the failure modes that are easy to get wrong.

---

## 1 — Where things live under `cmake/`

| Path | Contents |
|---|---|
| `API.cmake` | The public `register_fprime_*` / `add_fprime_subdirectory` API |
| `module.cmake`, `config_assembler.cmake` | Internal module and configuration processing |
| `target/` | Built-in targets: `build`, `ut`, `dictionary`, `install`, `sbom`, `version` |
| `target/sub-build/` | Targets run in sub-builds (`fpp_locs`, `fpp_depend`, `module_info`) |
| `autocoder/` | Autocoder integration (`fpp.cmake`, `fpp_ut.cmake`) |
| `platform/` | `${FPRIME_PLATFORM}.cmake` platform files (Linux, Darwin, templates) |
| `toolchain/` | Cross-compile toolchain files |
| `settings/`, `options.cmake`, `flags.cmake`, `sanitizers.cmake` | Build settings, options, compile flags |
| `test/` | pytest suite for the build system itself |

Change the *narrowest* layer that solves the problem: a module's own
`CMakeLists.txt` first, then a platform/toolchain file, and only then
`cmake/` internals (which affect every project using F Prime).

---

## 2 — Which registration call

| Task | Call |
|---|---|
| Component, port, or plain library module | `register_fprime_module` |
| Deployment producing an F Prime binary | `register_fprime_deployment` |
| Non-deployment executable / tool | `register_fprime_executable` |
| Unit test | `register_fprime_ut` (see `fprime-unit-testing`) |
| Configuration or platform config module | `register_fprime_config` |
| Implementation of a swappable package (e.g. `Os_File`) | `register_fprime_module` + `IMPLEMENTS`; `register_os_implementation` for OSAL packages |
| Custom build target (`<target>` + `<MODULE>_<target>`) | `register_fprime_target` / `register_fprime_ut_target` |

Common directives: `SOURCES`, `AUTOCODER_INPUTS`, `HEADERS`, `DEPENDS`,
`REQUIRES_IMPLEMENTATIONS`, `CHOOSES_IMPLEMENTATIONS`. A new module
directory must also be pulled in with `add_fprime_subdirectory` from
the including `CMakeLists.txt`.

Guides: [customization / custom targets](../../../docs/user-manual/build-system/cmake-customization.md),
[implementations](../../../docs/user-manual/build-system/cmake-implementations.md),
[platforms](../../../docs/user-manual/build-system/cmake-platforms.md),
[toolchains](../../../docs/user-manual/build-system/cmake-toolchains.md),
[settings.ini](../../../docs/user-manual/build-system/settings.md),
[external libraries](../../../docs/how-to/integrate/integrate-external-libraries.md),
[OSAL implementation](../../../docs/how-to/integrate/implement-osal.md).

---

## 3 — Failure modes

- **Stale cache**: a build cache can go out of date in many ways —
  edited `settings.ini`, a new `CMakeLists.txt` or FPP file, a moved
  module, a switched toolchain. Symptoms are confusing missing-target,
  missing-autocode, or stale-dependency errors rather than compiler
  errors. Fix with `fprime-util generate -f` (`--ut` for the unit-test
  cache), which purges and regenerates the build cache.
- **Unresolved implementation**: every platform must `CHOOSES_IMPLEMENTATIONS`
  for every package a module `REQUIRES_IMPLEMENTATIONS`; a missing
  choice surfaces as a link error, not a CMake error. Use the `_Stub`
  implementation when a platform lacks the capability; override per
  deployment, executable, or UT only.
- **Sub-build assumptions**: code that must not run during the
  `fpp_locs`/`fpp_depend` sub-builds needs `skip_on_sub_build()`;
  platform-specific modules need `restrict_platforms(...)`.
- **Hardcoded host paths or tool locations** in a toolchain belong in
  an `environment_file`, not in committed CMake.
- **Bypassing the API** (raw `add_library`/`target_link_libraries` for
  an F Prime module) loses autocoding, dictionary, and UT integration.
