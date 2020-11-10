#!/usr/bin/env python3

import os
import shutil


##
## Copied directly from the Python Cookbook
##
## Check if file exists, and create a backup copy using an appended
## versioning string. This function was copied directly from the
## language cookbook.
##
## Arguments:
##    filename: the test filename
##    vtype: copy or rename action
##
## Return:
##    True if action taken; False otherwise
##
## Exceptions:
##    ValueError - unknown vtype (copy, or rename)
##    RuntimeError - all extensions used (eg: filename.c.999)
##
def VersionFile(filename, vtype="copy"):

    if os.path.isfile(filename):

        # check the vtype parameter
        if vtype not in ("copy", "rename"):
            raise ValueError("Unknown vtype {!r}".format(vtype))

        # Determine root filename so extension doesn't get longer
        n, e = os.path.splitext(filename)

        # Is e a three-digit integer preceded by a dot
        if len(e) == 4 and e[1:].isdigit():
            num = 1 + int(e[1:])
            root = n
        else:
            num = 0
            root = filename

        # Find the next available file version
        for i in range(num, 1000):

            new_file = "%s.%03d" % (root, i)

            if not os.path.exists(new_file):
                if vtype == "copy":
                    shutil.copy(filename, new_file)
                else:
                    os.rename(filename, new_file)
                return True

        raise RuntimeError("Can't {} {!r}, all name taken".format(vtype, filename))

    return False


if __name__ == "__main__":
    # Create a dummy file 'test.txt'
    tfn = "test.txt"
    open(tfn, "w").close()

    # version it 3 times
    print(VersionFile(tfn))
    print(VersionFile(tfn))
    print(VersionFile(tfn))

    # remove all test.txt* files we just made
    for x in ("", ".000", ".001", ".002"):
        os.unlink(tfn + x)

    # show what happens when the file does not exist
    print(VersionFile(tfn))
    print(VersionFile(tfn))
