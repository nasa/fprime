####
# test_basic.py:
#
# Basic CMake tests.
#
####
import platform
import json

import settings

import cmake

_ = cmake.get_build(
    "FEATURE_BUILD",
    settings.DATA_DIR / "TestDeployment",
    {
        "FPRIME_FRAMEWORK_PATH": settings.REF_APP_PATH.parent,
        "FPRIME_PROJECT_ROOT": settings.DATA_DIR,
        "FPRIME_LIBRARY_LOCATIONS": ";".join(
            [
                str(settings.DATA_DIR / "test-fprime-library"),
                str(settings.DATA_DIR / "test-fprime-library2"),
            ]
        ),
    },
    make_targets=[
        "TestDeployment",
        "test",
        "TestDeployment_test",
        "TestLibrary_TestComponent_test",
        "version",
    ],
)


def test_feature_run(FEATURE_BUILD):
    """Basic run test for feature build"""
    cmake.assert_process_success(FEATURE_BUILD)


def test_feature_framework(FEATURE_BUILD):
    """Feature build check framework properly detected"""
    cmake.assert_process_success(FEATURE_BUILD)
    for module in settings.FRAMEWORK_MODULES + ["Svc_CmdDispatcher"]:
        library_name = f"lib{module}.a"
        output_path = FEATURE_BUILD["build"] / "lib" / platform.system() / library_name
        assert output_path.exists(), f"Failed to locate {library_name} in build output"


def test_feature_library(FEATURE_BUILD):
    """Feature build check libraries properly detected"""
    cmake.assert_process_success(FEATURE_BUILD)
    modules = ["TestLibrary_TestComponent", "TestLibrary2_TestComponent"]
    for module in modules:
        library_name = f"lib{module}.a"
        output_path = FEATURE_BUILD["build"] / "lib" / platform.system() / library_name
        assert output_path.exists(), f"Failed to locate {library_name} in build output"


def test_feature_deployment(FEATURE_BUILD):
    """Feature build check deployment properly detected"""
    cmake.assert_process_success(FEATURE_BUILD)
    library_name = "TestDeployment"
    output_path = FEATURE_BUILD["build"] / "bin" / platform.system() / library_name
    assert output_path.exists(), f"Failed to locate {library_name} in build output"


def test_feature_autocoder(FEATURE_BUILD):
    """Feature build check deployment properly detected"""
    cmake.assert_process_success(FEATURE_BUILD)
    for output_file in ["test-ac-1", "test-ac-2"]:
        output_path = FEATURE_BUILD["build"] / output_file
        assert output_path.exists(), f"Failed to locate {output_file} in build output"


def test_feature_targets(FEATURE_BUILD):
    """Feature build check deployment properly detected"""
    cmake.assert_process_success(FEATURE_BUILD)
    for output_file in [
        "global-test",
        "deployment-test",
        "TestLibrary_TestComponent-test",
    ]:
        output_path = FEATURE_BUILD["build"] / output_file
        assert output_path.exists(), f"Failed to locate {output_file} in build output"


def test_feature_version_info(FEATURE_BUILD):
    """Build and assert version files validity"""
    cmake.assert_process_success(FEATURE_BUILD)
    version_hpp = FEATURE_BUILD["build"] / "versions" / "version.hpp"
    version_json = FEATURE_BUILD["build"] / "versions" / "version.json"
    assert version_hpp.exists(), "Failed to locate version.hpp in build output"
    assert version_json.exists(), "Failed to locate version.json in build output"
    versions_dict = json.loads(version_json.read_text())
    for key in ["framework_version", "project_version", "library_versions"]:
        assert key in versions_dict, f"Failed to locate key: {key} in version.json"
    assert (
        "test-fprime-library" in versions_dict["library_versions"]
    ), "Library version missing "
    assert (
        "test-fprime-library2" in versions_dict["library_versions"]
    ), "Library version missing"
    assert (
        versions_dict["library_versions"]["test-fprime-library"]
        == versions_dict["framework_version"]
    ), "Library version mismatch"
    assert (
        versions_dict["library_versions"]["test-fprime-library2"]
        == versions_dict["framework_version"]
    ), "Library version mismatch"


def test_feature_installation(FEATURE_BUILD):
    """Run reference and assert reference targets exit"""
    cmake.assert_process_success(FEATURE_BUILD)
    deployment_name = "TestDeployment"
    for module in settings.FRAMEWORK_MODULES + [
        "Svc_CmdDispatcher",
        "TestLibrary_TestComponent",
        "TestLibrary2_TestComponent",
    ]:
        library_name = f"lib{module}.a"
        output_path = (
            FEATURE_BUILD["install"]
            / platform.system()
            / deployment_name
            / "lib"
            / "static"
            / library_name
        )
        assert output_path.exists(), f"Failed to locate {library_name} in build output"
    output_path = (
        FEATURE_BUILD["install"]
        / platform.system()
        / deployment_name
        / "bin"
        / deployment_name
    )
    assert output_path.exists(), "Failed to locate TestDeployment in build output"


def test_sub_build(FEATURE_BUILD):
    """Test that the sub buil process builds"""
    output_paths = [
        # Test that a file in the sub build exists
        FEATURE_BUILD["build"] / f"sub-build-test-sub-build" / "sub-test",
        # Test the sub build could "return" files to the primary build
        FEATURE_BUILD["build"] / "sub-test",
    ]
    for output_path in output_paths:
        assert output_path.exists(), "Failed to locate sub-build artifact"
