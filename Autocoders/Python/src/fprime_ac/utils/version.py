""" fprime version handling and reporting """
import os
from setuptools_scm import get_version

ROOT_PARENT_COUNT = 5


def get_fprime_version():
    """Gets the fprime version using setuptools_scm"""
    # First try to read the SCM version
    try:
        return get_version(
            root=os.sep.join([".."] * ROOT_PARENT_COUNT), relative_to=__file__
        )
    # Fallback to a specified version when SCM is unavailable
    except LookupError:
        return "1.5.4"  # Must be kept up-to-date when tagging
