"""
fprime.build:

Sets up a singleton that handles the calls for build system help. This will allow for support of future build systems
that are not CMake.
"""
from .cmake import CMakeHandler

# _BUILDER is a singleton supported by a CMakeHandler, it should be used via fprime.fbuild.builder()
_BUILDER = None


def builder():
    """
    Gets the singleton builder from the module. If not created yet, one will be built and then returned.
    """
    global _BUILDER  # For use with singleton pattern
    if _BUILDER is None:
        _BUILDER = CMakeHandler()
    return _BUILDER
