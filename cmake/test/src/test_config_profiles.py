"""Check what CI configuration profiles actually compile, rather than cache."""

import json
import shlex
import subprocess

import pytest
import yaml

from . import settings

ROOT = settings.FRAMEWORK_PATH.resolve()
HOOK = ROOT / "ci" / "apply-config-profile.cmake"
PROFILES = json.loads((ROOT / "ci" / "config-profiles.json").read_text())

# Independent expectations: do not derive the test oracle from generate-args.
DEFAULTS = {
    "FW_OBJECT_NAMES": 1,
    "FW_OBJECT_REGISTRATION": 1,
    "FW_QUEUE_REGISTRATION": 1,
    "FW_PORT_TRACING": 1,
    "FW_ENABLE_TEXT_LOGGING": 1,
    "FW_SERIALIZABLE_TO_STRING": 1,
    "FW_ASSERT_LEVEL": 3,
    "FW_PORT_SERIALIZATION": 1,
    "FW_ASSERTIONS_ALWAYS_ABORT": 0,
    "FW_CMD_CHECK_RESIDUAL": 1,
}
OVERRIDES = {
    "minimal": {
        "FW_OBJECT_NAMES": 0,
        "FW_OBJECT_REGISTRATION": 0,
        "FW_QUEUE_REGISTRATION": 0,
        "FW_PORT_TRACING": 0,
        "FW_ENABLE_TEXT_LOGGING": 0,
        "FW_SERIALIZABLE_TO_STRING": 0,
    },
    "no-text-logging": {"FW_ENABLE_TEXT_LOGGING": 0, "FW_SERIALIZABLE_TO_STRING": 0},
    "no-object-names": {"FW_OBJECT_NAMES": 0},
    "direct-port-calls": {},
    "fileid-assert": {"FW_ASSERT_LEVEL": 2},
    "no-assert": {"FW_ASSERT_LEVEL": 1},
    "relative-path-assert": {"FW_ASSERT_LEVEL": 4},
    "no-port-serialization": {"FW_PORT_SERIALIZATION": 0},
    "assertions-always-abort": {"FW_ASSERTIONS_ALWAYS_ABORT": 1},
    "no-cmd-residual-check": {"FW_CMD_CHECK_RESIDUAL": 0},
}


def run(*args):
    """Capture diagnostics without depending on a particular CMake generator."""
    return subprocess.run(
        args, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, timeout=90
    )


def project(directory, overrides):
    """Compile real FpConfig.h and Unix platform headers, without stubs."""
    directory.mkdir(parents=True, exist_ok=True)
    (directory / "child").mkdir(exist_ok=True)
    (directory / "CMakeLists.txt").write_text(
        """cmake_minimum_required(VERSION 3.18)
project(ConfigProfileProbe LANGUAGES C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include_directories("${FRAMEWORK}" "${FRAMEWORK}/default"
                    "${FRAMEWORK}/cmake/platform/unix")
add_compile_definitions(PARENT_SENTINEL=23)
# These are build-system options, not compiler definitions.
if(DEFINED FPRIME_ENABLE_TEXT_LOGGERS)
    message(STATUS "Text logger selection: ${FPRIME_ENABLE_TEXT_LOGGERS}")
endif()
if(DEFINED FPRIME_ENABLE_DIRECT_PORT_CALLS)
    message(STATUS "Direct port selection: ${FPRIME_ENABLE_DIRECT_PORT_CALLS}")
endif()
add_library(root_probe OBJECT probe.c probe.cpp)
add_subdirectory(child)
"""
    )
    # Another project() invokes the hook again and exercises include_guard.
    (directory / "child" / "CMakeLists.txt").write_text(
        "project(ChildProbe LANGUAGES C CXX)\n"
        "add_library(child_probe OBJECT ../probe.c ../probe.cpp)\n"
    )
    expected = {**DEFAULTS, **overrides}
    expected["FW_OBJECT_TO_STRING"] = expected["FW_OBJECT_NAMES"]
    expected.update(PARENT_SENTINEL=23, USER_SENTINEL=7)
    source = "#include <config/FpConfig.h>\n"
    for name, value in expected.items():
        source += f"#if {name} != {value}\n#error Unexpected_{name}\n#endif\n"
    for suffix in ("c", "cpp"):
        (directory / f"probe.{suffix}").write_text(source)
    return directory


def configure(source, build, args=(), hook=True):
    options = [
        f"-DFRAMEWORK={ROOT}",
        "-DCMAKE_C_FLAGS=-DUSER_SENTINEL=7",
        "-DCMAKE_CXX_FLAGS=-DUSER_SENTINEL=7",
    ]
    if hook:
        options.append(f"-DCMAKE_PROJECT_INCLUDE={HOOK}")
    return run("cmake", "-S", str(source), "-B", str(build), *options, *args)


def build_success(build):
    result = run("cmake", "--build", str(build), "--parallel", "2")
    assert result.returncode == 0, result.stdout
    commands = json.loads((build / "compile_commands.json").read_text())
    assert len(commands) == 4, "Both languages must compile in both project scopes"
    return commands


@pytest.mark.parametrize("profile", PROFILES, ids=lambda p: p["name"])
def test_profile_compiler_values(tmp_path, profile):
    """Include disabled profiles without changing their workflow selection."""
    expected = OVERRIDES[profile["name"]]
    source = project(tmp_path / "source with spaces", expected)
    build = tmp_path / "build with spaces"
    result = configure(source, build, shlex.split(profile["generate-args"]))
    assert result.returncode == 0, result.stdout
    for command in build_success(build):
        args = shlex.split(command["command"])
        for name, value in expected.items():
            assert args.count(f"-D{name}={value}") == 1
        assert not any(arg.startswith("-DFPRIME_") for arg in args)


@pytest.mark.parametrize("hook", [False, True])
def test_default_compiler_values(tmp_path, hook):
    source = project(tmp_path / "source", {})
    build = tmp_path / "build"
    result = configure(source, build, hook=hook)
    assert result.returncode == 0, result.stdout
    build_success(build)


def test_original_arguments_leave_default_enabled(tmp_path):
    """A no-hook build reproduces #5908 in both C and C++."""
    source = project(tmp_path / "source", {"FW_OBJECT_NAMES": 0})
    build = tmp_path / "build"
    result = configure(source, build, ["-DFW_OBJECT_NAMES=0"], hook=False)
    assert result.returncode == 0, result.stdout
    result = run("cmake", "--build", str(build), "--parallel", "2")
    assert result.returncode != 0
    assert "Unexpected_FW_OBJECT_NAMES" in result.stdout


@pytest.mark.parametrize("remove_hook", [False, True])
def test_removed_override_does_not_survive_reconfigure(tmp_path, remove_hook):
    source = project(tmp_path / "source", {"FW_OBJECT_NAMES": 0})
    build = tmp_path / "build"
    result = configure(source, build, ["-DFW_OBJECT_NAMES=0"])
    assert result.returncode == 0, result.stdout
    build_success(build)
    project(source, {})
    result = configure(
        source,
        build,
        ["-UCMAKE_PROJECT_INCLUDE"] if remove_hook else ["-UFW_OBJECT_NAMES"],
        hook=not remove_hook,
    )
    assert result.returncode == 0, result.stdout
    commands = build_success(build)
    assert all("-DFW_OBJECT_NAMES=" not in c["command"] for c in commands)


@pytest.mark.parametrize(
    "argument, diagnostic",
    [
        ("-DFW_OBJECT_NAMES=ON", "Invalid config-test value"),
        ("-DFW_OBJECT_NAMES=-1", "Invalid config-test value"),
        ("-DFW_ASSERT_LEVEL=0", "Invalid config-test value"),
        ("-DFW_ASSERT_LEVEL=5", "Invalid config-test value"),
        ("-DFW_OBJECT_NAMES=0;OTHER_DEFINE=1", "Invalid config-test value"),
        ("-DFW_UNKNOWN_SWITCH=0", "Unsupported config-test switch"),
    ],
)
def test_invalid_switch_fails_configuration(tmp_path, argument, diagnostic):
    source = project(tmp_path / "source", {})
    result = configure(source, tmp_path / "build", [argument])
    assert result.returncode != 0
    assert diagnostic in result.stdout


@pytest.mark.parametrize("name", ["config-test.yml", "config-test-nightly.yml"])
def test_every_workflow_generation_loads_hook(name):
    workflow = yaml.safe_load((ROOT / ".github" / "workflows" / name).read_text())
    checked = 0
    for job in ("config-build", "config-ut"):
        for step in workflow["jobs"][job]["steps"]:
            arguments = step.get("with", {}).get("generate-args")
            if arguments is not None:
                assert "${{ matrix.generate-args }}" in arguments
                assert (
                    "-DCMAKE_PROJECT_INCLUDE=${{ github.workspace }}"
                    "/ci/apply-config-profile.cmake"
                ) in arguments
                checked += 1
    assert checked == 4
