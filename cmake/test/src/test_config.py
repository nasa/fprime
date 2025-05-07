####
# test_basic.py:
#
# Basic CMake tests.
#
####
from . import cmake
from . import settings

_ = cmake.get_build(
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
        "CMAKE_DEBUG_OUTPUT": "ON",
    },
    make_targets=["TestModelOverride", "TestHeaderOverride", "TestFPrimeLibraryOverride", "library_config", "TestLibraryNewConfig"],
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