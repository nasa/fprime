####
# test_basic.py:
#
# Basic CMake tests.
#
####
import platform

import settings

import cmake

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
    output_path = (
        IMPLEMENTATION_TEST["build"]
        / "lib"
        / platform.system()
        / "libTest_Implementation_Platform.a"
    )
    assert (
        output_path.exists()
    ), f"Failed to build the platform-specified implementation target"


def test_override_implementation(IMPLEMENTATION_TEST):
    """Check the platform-specified implementation was produced"""
    cmake.assert_process_success(IMPLEMENTATION_TEST)
    output_path = (
        IMPLEMENTATION_TEST["build"]
        / "lib"
        / platform.system()
        / "libTest_Override_Override.a"
    )
    assert output_path.exists(), f"Failed to build the override implementation target"


def test_non_built_implementation(IMPLEMENTATION_TEST):
    """Check the override target that wasn't use was not built along with the override platform target"""
    cmake.assert_process_success(IMPLEMENTATION_TEST)
    output_path = (
        IMPLEMENTATION_TEST["build"]
        / "lib"
        / platform.system()
        / "libTest_Platform_Override.a"
    )
    assert not output_path.exists(), f"Failed to ignore non-built override target"
    output_path = (
        IMPLEMENTATION_TEST["build"]
        / "lib"
        / platform.system()
        / "libTest_Override_Unused.a"
    )
    assert not output_path.exists(), f"Failed to ignore overridden target"
