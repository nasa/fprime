from . import cmake
from .test_feature import FEATURE_BUILD_RESULT


def test_relative_paths(FEATURE_BUILD):
    """Basic run test for feature build"""
    cmake.assert_process_success(FEATURE_BUILD)
