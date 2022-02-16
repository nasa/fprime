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
    for item in sys.path:
        item = os.path.abspath(item)

        if sys.platform == "win32":
            item = item.lower()

        if new_path in (item, item + os.sep):
            return 0

    sys.path.append(new_path)

    return 1


if __name__ == "__main__":

    print("Before:")
    for item in sys.path:
        print(item)

    if sys.platform == "win32":
        print(AddSysPath("c:\\Temp"))
        print(AddSysPath("c:\\temp"))
    else:
        print(AddSysPath("/usr/lib"))

    print("After:")
    for item in sys.path:
        print(item)
