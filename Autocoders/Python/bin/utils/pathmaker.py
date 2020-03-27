####
# pathmaker.py:
#
# The purpose of this file is to setup the F prime path automatically as part of a run of F prime. This should prevent
# the user from needing to do the same thing, and allow for a pure-Python way of running F prime tools.
####
import os
import sys

def setup_fprime_autocoder_path():
    """
    Sets up the system path to include F prime autocoder directories. This is useful for all AC scripts.
    """
    FW_PACKAGE = os.path.join(os.path.dirname(__file__), "..", "..", "..", "Fw", "Python", "src")
    AC_PACKAGE = os.path.join(os.path.dirname(__file__), "..", "..", "src")
    sys.path.append(FW_PACKAGE)
    sys.path.append(AC_PACKAGE)
