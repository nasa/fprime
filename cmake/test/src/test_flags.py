####
# test_flags.py:
#
# Tests that the compile flags F Prime requires reach a project that consumes it, rather than only
# applying when F Prime is the top-level project.
####
from . import cmake
from . import settings

_ = cmake.get_build(
    "FLAGS_BUILD",
    settings.DATA_DIR / "TestFlagsProject",
    {"CMAKE_CXX_FLAGS": "-pedantic -Werror"},
    make_targets=["TestFlags_FlagsComponent"],
)


def test_required_flags_reach_consuming_project(FLAGS_BUILD):
    """Test that a consuming project can hold itself to a strict warning set

    The dispatch code generated for an async serial port declares a variable length array, which
    -pedantic rejects. Building this project therefore only works when the flags F Prime requires
    arrive with the framework rather than being set by whoever happens to be the top-level project.
    """
    cmake.assert_process_success(
        FLAGS_BUILD, targets=["TestFlags_FlagsComponent"], warnings_ok=True
    )
