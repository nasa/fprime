import os
import time
from pathlib import Path

from . import cmake
from . import settings

_ = cmake.get_build(
    "AUTOCODER_BUILD",
    settings.DATA_DIR / "TestDeployment",
    {
        "FPRIME_FRAMEWORK_PATH": settings.FRAMEWORK_PATH,
        "FPRIME_PROJECT_ROOT": settings.DATA_DIR,
        "FPRIME_LIBRARY_LOCATIONS": ";".join(
            [
                str(settings.DATA_DIR / "test-fprime-library"),
                str(settings.DATA_DIR / "test-fprime-library2"),
            ]
        ),
    },
    make_targets=[
        "TestBuildAutocoderModule",
        "TestTargetAutocoderModule",
        "TestChainedAutocoderModule",
        "TestHeaderAutocoderModule",
        "TestRelative",
    ],
)


def test_build_autocoder(AUTOCODER_BUILD):
    """Test that a build-autocoder works"""
    cmake.assert_process_success(AUTOCODER_BUILD, targets=["TestBuildAutocoderModule"])


def test_target_autocoder(AUTOCODER_BUILD):
    """Test that a target-triggered autocoder works"""
    cmake.assert_process_success(AUTOCODER_BUILD, targets=["TestTargetAutocoderModule"])


def test_autocoder_non_build_files(AUTOCODER_BUILD):
    """Test that a target-triggered autocoder works"""
    cmake.assert_process_success(AUTOCODER_BUILD, targets=["TestTargetAutocoderModule"])
    build_cache_path = (
        AUTOCODER_BUILD["build"] / "TestDeployment" / "TestTargetAutocoder"
    )
    for created in [
        "test1.test-target.generated.txt",
        "test2.test-target.generated.txt",
    ]:
        full_created_path = build_cache_path / created
        assert (
            full_created_path.exists()
        ), f"Failed to create non-build output: {created}"


def test_autocoder_chaining(AUTOCODER_BUILD):
    """Test that autocoder chaining works - where one autocoder's output becomes another's input"""
    cmake.assert_process_success(
        AUTOCODER_BUILD, targets=["TestChainedAutocoderModule"]
    )
    build_cache_path = (
        AUTOCODER_BUILD["build"] / "TestDeployment" / "TestChainedAutocoder"
    )

    # Verify that chained autocoder files are created
    # First autocoder should create intermediate files with .generated suffix
    for intermediate in [
        "test1.test-target.generated.txt",
        "test2.test-target.generated.txt",
    ]:
        intermediate_path = build_cache_path / intermediate
        assert (
            intermediate_path.exists()
        ), f"Failed to create intermediate autocoder output: {intermediate}"

    # Second autocoder should process the intermediate files and create final outputs
    for final in ["test1.chained.txt", "test2.chained.txt"]:
        final_path = build_cache_path / final
        assert (
            final_path.exists()
        ), f"Failed to create chained autocoder output: {final}"


def test_autocoder_header_as_sources(AUTOCODER_BUILD):
    """Test that autocoders can generate header files that are treated as sources"""
    cmake.assert_process_success(AUTOCODER_BUILD, targets=["TestHeaderAutocoderModule"])


def test_autocoder_rerun_autocoder(AUTOCODER_BUILD):
    """Test that autocoders can generate header files that are treated as sources"""
    cmake.assert_process_success(AUTOCODER_BUILD, targets=["TestBuildAutocoderModule"])
    build_cache_path = (
        AUTOCODER_BUILD["build"] / "TestDeployment" / "TestBuildAutocoder"
    )
    rerun_autocoder_output = build_cache_path / "test-rerun-autocoder.txt"
    assert rerun_autocoder_output.exists(), "Failed to create rerun autocoder output"


def _configure_depends(build):
    """Path of the configure-dependency listing written by TestRelative"""
    return build / "TestDeployment" / "TestRelative" / "configure-depends.txt"


def test_included_fpp_is_tracked(AUTOCODER_BUILD):
    """Test that an .fppi pulled in by a model file is a configure dependency

    A change to an included file has to re-run generation, so the file must be
    tracked by CMake rather than only by the generated output's own rule.
    """
    cmake.assert_process_success(AUTOCODER_BUILD, targets=["TestRelative"])
    listing = _configure_depends(AUTOCODER_BUILD["build"])
    assert listing.exists(), "TestRelative did not record its configure dependencies"
    assert "included.fppi" in listing.read_text(), (
        "included.fppi is not a configure dependency, so editing it will not "
        "re-run generation"
    )


def test_touching_included_fpp_reconfigures(AUTOCODER_BUILD):
    """Test that editing an .fppi actually triggers a reconfigure

    The listing above is rewritten on every configure, so its timestamp moving
    is the evidence that touching the include re-ran CMake.
    """
    build = AUTOCODER_BUILD["build"]
    listing = _configure_depends(build)
    assert listing.exists(), "TestRelative did not record its configure dependencies"

    # Settle first: a build with nothing to do must not move the timestamp.
    cmake.run_make(build, "TestRelative")
    settled = listing.stat().st_mtime
    cmake.run_make(build, "TestRelative")
    assert listing.stat().st_mtime == settled, "Build is not settled"

    include = Path(AUTOCODER_BUILD["source"]) / "TestRelative" / "included.fppi"
    os.utime(include, (time.time() + 1, time.time() + 1))
    return_code, _, _ = cmake.run_make(build, "TestRelative")

    assert return_code == 0, "Build failed after touching the included file"
    assert (
        listing.stat().st_mtime != settled
    ), "Touching included.fppi did not re-run CMake"
