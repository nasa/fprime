####
# test_platform.py:
#
# Tests for `fprime_find_platform_file` (cmake/platform/platform.cmake).
#
####
import re
import shutil
import tempfile
from pathlib import Path

from . import cmake
from . import settings

SOURCE_DIRECTORY = settings.DATA_DIR / "TestPlatformResolution"
PLATFORM_PATTERN = re.compile(r"\[test\] PLATFORM_FILE=(.+)")


def run_platform_case(platform, options=None):
    """Run CMake generation and return the selected platform file, if any."""
    build_directory = Path(tempfile.mkdtemp())
    cmake_options = {
        "FPRIME_FRAMEWORK_PATH": str(settings.FRAMEWORK_PATH),
        "FPRIME_PLATFORM": platform,
    }
    if options:
        cmake_options.update(options)
    try:
        return_code, stdout, _ = cmake.run_cmake(
            SOURCE_DIRECTORY, build_directory, cmake_options
        )
        matches = [PLATFORM_PATTERN.search(line) for line in stdout]
        platform_files = [Path(match.group(1)).resolve() for match in matches if match]
        return return_code, platform_files[0] if platform_files else None
    finally:
        shutil.rmtree(build_directory, ignore_errors=True)


def test_platform_project_direct():
    """A project-level cmake/platform file is discovered first."""
    return_code, platform_file = run_platform_case("ProjectDirect")
    expected = (SOURCE_DIRECTORY / "cmake/platform/ProjectDirect.cmake").resolve()
    assert return_code == 0, "CMake generation failed"
    assert platform_file == expected


def test_platform_project_library():
    """A platform file under PROJECT_SOURCE_DIR/lib/* is discovered."""
    return_code, platform_file = run_platform_case("ProjectLibrary")
    expected = (
        SOURCE_DIRECTORY / "lib/TestLibrary/cmake/platform/ProjectLibrary.cmake"
    ).resolve()
    assert return_code == 0, "CMake generation failed"
    assert platform_file == expected


def test_platform_project_subdirectory():
    """A platform file under a project subdirectory is discovered."""
    return_code, platform_file = run_platform_case("ProjectSubdirectory")
    expected = (
        SOURCE_DIRECTORY
        / "TestSubdirectory/cmake/platform/ProjectSubdirectory.cmake"
    ).resolve()
    assert return_code == 0, "CMake generation failed"
    assert platform_file == expected


def test_platform_framework_fallback():
    """The framework platform directory remains the final fallback."""
    return_code, platform_file = run_platform_case("Linux")
    expected = (settings.FRAMEWORK_PATH / "cmake/platform/Linux.cmake").resolve()
    assert return_code == 0, "CMake generation failed"
    assert platform_file == expected


def test_platform_ignores_fprime_project_root():
    """FPRIME_PROJECT_ROOT is no longer searched for platform files."""
    with tempfile.TemporaryDirectory() as legacy_root:
        legacy_file = Path(legacy_root) / "cmake/platform/LegacyProjectRoot.cmake"
        legacy_file.parent.mkdir(parents=True)
        legacy_file.write_text("# legacy platform\n", encoding="utf-8")
        return_code, platform_file = run_platform_case(
            "LegacyProjectRoot", {"FPRIME_PROJECT_ROOT": legacy_root}
        )
    assert return_code != 0, "Legacy project root should not resolve a platform"
    assert platform_file is None


def test_platform_ignores_fprime_library_locations():
    """FPRIME_LIBRARY_LOCATIONS is no longer searched for platform files."""
    with tempfile.TemporaryDirectory() as legacy_library:
        legacy_file = Path(legacy_library) / "cmake/platform/LegacyLibrary.cmake"
        legacy_file.parent.mkdir(parents=True)
        legacy_file.write_text("# legacy platform\n", encoding="utf-8")
        return_code, platform_file = run_platform_case(
            "LegacyLibrary", {"FPRIME_LIBRARY_LOCATIONS": legacy_library}
        )
    assert return_code != 0, "Legacy library location should not resolve a platform"
    assert platform_file is None
