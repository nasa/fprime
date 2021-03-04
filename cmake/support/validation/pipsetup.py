"""
pipsetup.py:

A validation script checking that the pip setup is in-place properly. This means that the python
packages are installed properly.

@author mstarch
"""
from __future__ import print_function
import sys

# Migrated from setup.py in Fw/Python
REQUIRED_PACKS = ["lxml", "Markdown", "pexpect", "pytest", "Cheetah3"]


def validate_python_setup():
    """
    Validates the python setup of the system to ensure that the CMake system, Autocoder, etc can run
    as expected.
    """
    if sys.version_info[0] < 3 or sys.version_info[1] < 5:
        print("Python 3.5+ required")
        return False
    import pkg_resources

    deps = REQUIRED_PACKS
    # Checking all dependencies one-at-a-time
    good = True
    for dep in deps:
        try:
            pkg_resources.require([dep])
        except pkg_resources.DistributionNotFound:
            print(dep.strip(), "is missing.")
            good = False
        except pkg_resources.VersionConflict:
            print(dep.strip(), "is not installed version.")
            good = False
        except Exception as e:
            if "RequirementParseError" not in str(e):
                raise
    return good


if __name__ == "__main__":
    sys.exit(0 if validate_python_setup() else 1)
