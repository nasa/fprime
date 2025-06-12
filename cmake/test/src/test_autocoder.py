from . import cmake
from . import settings

_ = cmake.get_build(
    "AUTOCODER_BUILD",
    settings.DATA_DIR / "TestDeployment",
    {
        "FPRIME_FRAMEWORK_PATH": settings.REF_APP_PATH.parent,
        "FPRIME_PROJECT_ROOT": settings.DATA_DIR,
        "FPRIME_LIBRARY_LOCATIONS": ";".join(
            [
                str(settings.DATA_DIR / "test-fprime-library"),
                str(settings.DATA_DIR / "test-fprime-library2"),
            ]
        )
    },
    make_targets=[],
)


def test_build_autocoder(AUTOCODER_BUILD):
    """Test that a build-autocoder workss"""
    cmake.assert_process_success(AUTOCODER_BUILD, targets=["TestBuildAutocoderModule"])


def test_target_autocoder(AUTOCODER_BUILD):
    """Test that a target-triggered autocoder works"""
    cmake.assert_process_success(AUTOCODER_BUILD, targets=["TestTargetAutocoderModule"])


def test_autocoder_non_build_files(AUTOCODER_BUILD):
    """Test that a target-triggered autocoder works"""
    cmake.assert_process_success(AUTOCODER_BUILD, targets=["TestTargetAutocoderModule"])
    build_cache_path = AUTOCODER_BUILD["build"] / "TestDeployment" / "TestTargetAutocoder"
    for created in ["test1.test-target.txt", "test2.test-target.txt"]:
        full_created_path = build_cache_path / created
        assert full_created_path.exists(), f"Failed to create non-build output: {created}"
