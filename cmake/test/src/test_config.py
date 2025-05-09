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
        "FPRIME_FRAMEWORK_PATH": settings.REF_APP_PATH.parent,
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
        "FPRIME_FRAMEWORK_PATH": settings.REF_APP_PATH.parent,
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
        "FPRIME_FRAMEWORK_PATH": settings.REF_APP_PATH.parent,
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


def test_library_bad_new_config(CONFIG_FAILED_NEW_FILE_BUILD):
    """Test that the new config that accidentally overrides work works"""
    with pytest.raises(AssertionError):
        cmake.assert_process_success(CONFIG_FAILED_NEW_FILE_BUILD, targets=[])


def test_library_bad_override_config(CONFIG_FAILED_OVERRIDE_BUILD):
    """Test that the config that is not an override overrides work works"""
    with pytest.raises(AssertionError):
        cmake.assert_process_success(CONFIG_FAILED_OVERRIDE_BUILD, targets=[])
