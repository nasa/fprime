""" fprime version handling and reporting """
import os
import subprocess

FALLBACK_VERSION = "v3.1.1"  # Keep up-to-date on release tag


def get_version_str(working_dir, fallback=FALLBACK_VERSION):
    """
    System call to get the version. It uses `git describe --tags --always` to get a standard version string as used
    across fprime. The working directory should be set to a (project, library, fprime) from which to obtain the version.

    Args:
        working_dir: working directory to introspect for version
        fallback: version to fallback to if git is not working
    Return:
        String containing the version for the given working directory
    """
    try:
        output = subprocess.check_output(
            ["git", "describe", "--tags", "--always"], cwd=working_dir
        )
        return output.strip().decode("ascii")
    except Exception:
        return fallback


def get_fprime_version():
    """Calculate the fprime framework version

    Calculate the fprime framework version. This uses `get_version_str` and fallback to the FALLBACK_VERSION as
    specified within this file.

    Return:
        Version of fprime framework
    """
    fprime_directory = os.environ.get(
        "FPRIME_FRAMEWORK_PATH", os.path.dirname(__file__)
    )
    return get_version_str(working_dir=fprime_directory, fallback=FALLBACK_VERSION)


def get_project_version(fallback=FALLBACK_VERSION):
    """Calculate the fprime project version

    Calculate the fprime project version. This uses `get_version_str` and fallback to the FALLBACK_VERSION as
    specified within this file unless the fallback argument was passed in.

    Args:
        fallback: version to fallback to if git is not working

    Return:
        Version of fprime framework
    """
    fprime_directory = os.environ.get("FPRIME_PROJECT_ROOT", os.path.dirname(__file__))
    return get_version_str(working_dir=fprime_directory, fallback=fallback)
