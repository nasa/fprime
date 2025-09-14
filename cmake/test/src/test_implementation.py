####
# test_basic.py:
#
# Basic CMake tests.
#
####
import platform

from . import cmake
from . import settings


_ = cmake.get_build(
    "IMPLEMENTATION_TEST",
    settings.DATA_DIR / "test-implementations" / "Deployment",
    {
        "FPRIME_FRAMEWORK_PATH": settings.REF_APP_PATH.parent,
        "FPRIME_PROJECT_ROOT": settings.DATA_DIR / "test-implementations",
        "FPRIME_LIBRARY_LOCATIONS": ";".join(
            [
                str(settings.DATA_DIR / "test-implementations" / "test-platforms"),
            ]
        ),
    },
    make_targets=["Deployment"],
)


def test_platform_implementation(IMPLEMENTATION_TEST):
    """Check the platform-specified implementation was produced"""
    cmake.assert_process_success(IMPLEMENTATION_TEST)


def test_override_implementation(IMPLEMENTATION_TEST):
    """Check the override-specified implementation was produced"""
    cmake.assert_process_success(IMPLEMENTATION_TEST)


def test_non_built_implementation(IMPLEMENTATION_TEST):
    """Check the override target that wasn't use was not built along with the override platform target"""
    cmake.assert_process_success(IMPLEMENTATION_TEST)
