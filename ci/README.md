# Configuration profile checks

`config-profiles.json` supplies the named profiles used by the Config Test and
Config Test Nightly workflows. Each `generate-args` string contains CMake cache
arguments. A `-DFW_OBJECT_NAMES=0` cache entry alone does not change a C or C++
preprocessor macro.

Both workflows load `ci/apply-config-profile.cmake` using CMake's
`CMAKE_PROJECT_INCLUDE` hook for every framework and reference-project build,
including unit-test generation. The hook validates the supported `FW_*` switches
and applies them with `add_compile_definitions`, including values of zero.
Definitions reach C, C++, generated sources and child directories. F Prime forwards
the hook and switch cache arguments to its separate configuration sub-builds.
`FPRIME_*` options continue through the normal build-system option handling.

The hook is opt-in and CI-only. It does not change normal framework defaults or
create a general-purpose `FW_*` command-line interface. It preserves compiler and
toolchain flags. Unsupported `FW_*` names or invalid values fail configuration
instead of silently selecting a default configuration. When adding a profile,
update the hook's supported switch list and the independent compiler expectations
in `cmake/test/src/test_config_profiles.py` as needed.

## Running a profile locally

From the repository root, after installing `requirements.txt` and initializing
submodules:

```sh
fprime-util generate --ut \
  -DCMAKE_PROJECT_INCLUDE="$PWD/ci/apply-config-profile.cmake" \
  -DFW_OBJECT_NAMES=0
```

Use a fresh build directory for each profile so cached switches do not carry into
the next profile. For a build from `TestDeploymentsProject`, supply the same
absolute hook path. Disabling or removing the hook requires a fresh configuration
before building without its definitions.

Run the profile regression tests with:

```sh
pytest -q cmake/test/src/test_config_profiles.py
```

The tests compile the actual framework configuration headers in C and C++, at the
root and in a nested project. They check each profile against independently stated
expected values, retain a no-hook reproduction, and verify both workflow call sites.
They also cover rejected inputs and reconfiguration without stale definitions.

Profile selection, disabled-profile reasons, job scheduling, permissions and
existing test execution remain unchanged. In particular, the disabled `minimal`
and `no-text-logging` entries are not enabled by this fix. This change also does not
resolve the separate empty pull-request matrix reported in issue #5677. Activating
previously ignored switches may reveal configuration-specific build or test defects;
those must be reported rather than reverting to the default configuration.
