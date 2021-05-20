""" fprime version handling and reporting """
import os
from pathlib import Path
from setuptools_scm import get_version

def get_fprime_version():
    """ Gets the fprime version using setuptools_scm  """
    version = Path(os.environ["FPRIME_FRAMEWORK_PATH"]) / "version.hpp"
    # First try to read the SCM version
    try:
        return get_version(root='.', relative_to=version)
    except LookupError:
        # Fallback on last C++ version
        with open(version, "r") as file_handle:
            version_points = [int(line.split("=")[-1].strip(" \n;")) for line in file_handle.readlines() if "=" in line and "const" in line]
        return "{}.{}.{}".format(*version_points)
