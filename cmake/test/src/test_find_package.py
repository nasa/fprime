####
# test_find_package.py:
#
# Tests that the CMake build system works with find_package(FPrime) and
# register_fprime_project() without specifying FPRIME_FRAMEWORK_PATH,
# FPRIME_LIBRARY_LOCATIONS, or FPRIME_PROJECT_ROOT on the command line.
####
from . import cmake
from . import settings


_ = cmake.get_build(
    "FIND_PACKAGE_BUILD",
    settings.DATA_DIR / "TestFindPackageDeployment",
    {
        "FPrime_DIR": str(settings.FRAMEWORK_PATH / "cmake"),
    },
    make_targets=["TestFindPackageDeployment"],
)


def test_find_package_build(FIND_PACKAGE_BUILD):
    """Test that find_package(FPrime) builds without specifying framework/library/project CLI vars"""
    cmake.assert_process_success(FIND_PACKAGE_BUILD, warnings_ok=True)
