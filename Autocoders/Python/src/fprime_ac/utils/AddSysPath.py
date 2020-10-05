#!/usr/bin/env python3

import os
import sys


##
## Copied directly from the Python Cookbook
##
## This function adds a directory to the Python sys.path value. It will
## not add the directory if it does not exist, or if it is already in the
## sys.path variable.
##
## Arguments:
##    new_path: The directory name to add
##
## Return:
##    1 if okay, -1 if path does not exist, 0 if already in the variable
##
def AddSysPath(new_path):

    # Avoid adding nonexistent paths
    if not os.path.exists(new_path):
        return -1

    # Standardize the path. Windows is case-sensitive
    new_path = os.path.abspath(new_path)

    if sys.platform == "win32":
        new_path = new_path.lower()

    # Check against all currently available paths
    for x in sys.path:
        x = os.path.abspath(x)

        if sys.platform == "win32":
            x = x.lower()

        if new_path in (x, x + os.sep):
            return 0

    sys.path.append(new_path)

    return 1


if __name__ == "__main__":

    print("Before:")
    for x in sys.path:
        print(x)

    if sys.platform == "win32":
        print(AddSysPath("c:\\Temp"))
        print(AddSysPath("c:\\temp"))
    else:
        print(AddSysPath("/usr/lib"))

    print("After:")
    for x in sys.path:
        print(x)
