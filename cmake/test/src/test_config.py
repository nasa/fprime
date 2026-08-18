####
# test_basic.py:
#
# Basic CMake tests.
#
####
import pytest
from . import cmake
from . import settings

_1 = cmake.get_build(
    "CONFIG_BUILD",
    settings.DATA_DIR / "TestConfigDeployment",
    {
        "FPRIME_FRAMEWORK_PATH": settings.FRAMEWORK_PATH,
        "FPRIME_PROJECT_ROOT": settings.DATA_DIR,
        "FPRIME_LIBRARY_LOCATIONS": ";".join(
            [
                str(settings.DATA_DIR / "test-config-library"),
            ]
        ),
    },
    make_targets=[
        "TestModelOverride",
        "TestHeaderOverride",
        "TestFPrimeLibraryOverride",
        "library_config",
        "TestLibraryNewConfig",
    ],
)

_2 = cmake.get_build(
    "CONFIG_FAILED_OVERRIDE_BUILD",
    settings.DATA_DIR / "TestConfigDeployment",
    {
        "FPRIME_FRAMEWORK_PATH": settings.FRAMEWORK_PATH,
        "FPRIME_PROJECT_ROOT": settings.DATA_DIR,
        "FPRIME_LIBRARY_LOCATIONS": ";".join(
            [
                str(settings.DATA_DIR / "test-config-library"),
            ]
        ),
        "_TEST_CONFIG_BAD_OVERRIDE": "ON",
    },
    make_targets=[],
)

_3 = cmake.get_build(
    "CONFIG_FAILED_NEW_FILE_BUILD",
    settings.DATA_DIR / "TestConfigDeployment",
    {
        "FPRIME_FRAMEWORK_PATH": settings.FRAMEWORK_PATH,
        "FPRIME_PROJECT_ROOT": settings.DATA_DIR,
        "FPRIME_LIBRARY_LOCATIONS": ";".join(
            [
                str(settings.DATA_DIR / "test-config-library"),
            ]
        ),
        "_TEST_CONFIG_BAD_NEW_FILE": "ON",
    },
    make_targets=[],
)


_4 = cmake.get_build(
    "CONFIG_INCLUDE_CONFLICT_BUILD",
    settings.DATA_DIR / "TestConfigConflictDeployment",
    {
        "FPRIME_FRAMEWORK_PATH": settings.FRAMEWORK_PATH,
        "FPRIME_PROJECT_ROOT": settings.DATA_DIR / "TestConfigConflictDeployment",
    },
    make_targets=[],
)


def test_fprime_model_override(CONFIG_BUILD):
    """Test that the config override works"""
    cmake.assert_process_success(CONFIG_BUILD, targets=["TestModelOverride"])


def test_fprime_header_override(CONFIG_BUILD):
    """Test that the config override works"""
    cmake.assert_process_success(CONFIG_BUILD, targets=["TestHeaderOverride"])


def test_fprime_library_override(CONFIG_BUILD):
    """Test that the config override (from library) works"""
    cmake.assert_process_success(CONFIG_BUILD, targets=["TestFPrimeLibraryOverride"])


def test_library_override(CONFIG_BUILD):
    """Test that the config override (of library) works"""
    cmake.assert_process_success(CONFIG_BUILD, targets=["library_config"])


def test_library_new_config(CONFIG_BUILD):
    """Test that the new config (of library) works"""
    cmake.assert_process_success(CONFIG_BUILD, targets=["TestLibraryNewConfig"])


def test_override_survives_reconfigure(CONFIG_BUILD):
    """Test that a re-configure does not disturb an overridden configuration file

    An overridden destination has two candidate sources, the original and the
    override. Writing both leaves the file rewritten on every configure, which
    moves its timestamp and rebuilds everything including it.
    """
    overridden = CONFIG_BUILD["build"] / "F-Prime" / "default" / "config" / "DpCfg.hpp"
    assert overridden.exists(), "Override destination was never produced"
    before = overridden.stat().st_mtime
    contents = overridden.read_bytes()
    override_source = CONFIG_BUILD["source"] / "override" / "project" / "DpCfg.hpp"
    assert (
        contents == override_source.read_bytes()
    ), "Destination does not hold the override's content"

    return_code, _, _ = cmake.run_cmake(CONFIG_BUILD["source"], CONFIG_BUILD["build"])

    assert return_code == 0, "Re-configure failed"
    assert overridden.read_bytes() == contents, "Re-configure changed the override"
    assert (
        overridden.stat().st_mtime == before
    ), "Re-configure rewrote the override, forcing a rebuild of everything using it"


def test_override_removal_restores_original(CONFIG_BUILD):
    """Test that dropping an override puts F Prime's original file back

    Deferring the write makes the destination depend on a map of recorded
    sources. That map is rebuilt from scratch on every configure, so a
    destination that is no longer overridden must revert rather than keep the
    override that is gone.
    """
    overridden = CONFIG_BUILD["build"] / "F-Prime" / "default" / "config" / "DpCfg.hpp"
    original = settings.FRAMEWORK_PATH / "default" / "config" / "DpCfg.hpp"
    assert (
        overridden.read_bytes() != original.read_bytes()
    ), "Override was never applied"

    try:
        return_code, _, _ = cmake.run_cmake(
            CONFIG_BUILD["source"],
            CONFIG_BUILD["build"],
            {"_TEST_CONFIG_DROP_HEADER_OVERRIDE": "ON"},
        )
        assert return_code == 0, "Re-configure without the override failed"
        assert (
            overridden.read_bytes() == original.read_bytes()
        ), "Destination kept the removed override instead of F Prime's original"
    finally:
        # Leave the build as the other tests expect to find it
        cmake.run_cmake(
            CONFIG_BUILD["source"],
            CONFIG_BUILD["build"],
            {"_TEST_CONFIG_DROP_HEADER_OVERRIDE": "OFF"},
        )


def test_library_bad_new_config(CONFIG_FAILED_NEW_FILE_BUILD):
    """Test that the new config that accidentally overrides work works"""
    with pytest.raises(AssertionError):
        cmake.assert_process_success(CONFIG_FAILED_NEW_FILE_BUILD, targets=[])


def test_library_bad_override_config(CONFIG_FAILED_OVERRIDE_BUILD):
    """Test that the config that is not an override overrides work works"""
    with pytest.raises(AssertionError):
        cmake.assert_process_success(CONFIG_FAILED_OVERRIDE_BUILD, targets=[])


def test_config_include_path_conflict(CONFIG_INCLUDE_CONFLICT_BUILD):
    """Test that config files with conflicting source and build cache include paths fail"""
    with pytest.raises(AssertionError):
        cmake.assert_process_success(CONFIG_INCLUDE_CONFLICT_BUILD, targets=[])
