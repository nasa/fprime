#!/usr/bin/env python3
#
# File difference and rename routines.
# Main useful routines are:
#    DiffAndRename
#

import datetime
import logging
import os
import stat
import time

# Global logger initialization
PRINT = logging.getLogger("output")
DEBUG = logging.getLogger("debug")

removeJunk = False


def compare_except_lines(file1, file2, linesOkToBeDifferent):
    """
    Do a line by line compare of
    file1 and file2 ignoring lines
    specified by number (starting
    with line 0) in the list of lines.

    This routine mainly used for unit tests
    where ndiffs is 0 means files match.
    """
    missingFile = False
    if os.path.exists(file1) == False:
        PRINT("compare_and_rename: original file missing: %s" % file1)
        missingFile = True
    if os.path.exists(file2) == False:
        PRINT("compare_and_rename: new file missing: %s" % file2)
        missingFile = True
    if missingFile:
        return -1

    old = open(file1).readlines()
    new = open(file2).readlines()

    # print "LEN OLD = %d " % len(old)
    # print "LEN NEW = %d " % len(new)

    if len(old) != len(new):
        return abs(len(old) - len(new))

    for lineNum in range(0, len(old) - 1):
        if lineNum in linesOkToBeDifferent:
            pass
        else:
            oldLine = old[lineNum]
            newLine = new[lineNum]
            if oldLine != newLine:
                return 1
    return 0


def fileTimeTag(filename):
    """
    Generate a string as a time tag, consistent for renamed files
    """
    s = os.stat(filename)
    secs = s[stat.ST_MTIME]
    timeTuple = time.gmtime(secs)
    timeTag = ".%04d%02d%02dT%02d%02d%02d" % (
        timeTuple[0],
        timeTuple[1],
        timeTuple[2],
        timeTuple[3],
        timeTuple[4],
        timeTuple[5],
    )
    return timeTag


def compareAndRename(filename, dated_files_enable=False, except_lines_list=[]):
    """
    Compare filename with filename.new and if different overwright filename
    with filename.new. Do file diff on all lines if except_lines_list is []
    otherwise ignore lines in list when doing compare.

    When dated_files_enable=True write out files of form:
    filename.[new|old].yyyymmddThhmmss.

    @param filename: Name of file without the .new attached.
    @param dated_files_enable: Enable dated backup file generation.
    @param except_lines_list: List of lines in file to exclude from compare (start at zero).
    """
    # No .new file case.
    if not os.path.exists(filename + ".new"):
        str = "ERROR: Cannot find %s.new" % filename
        print(str)
        raise OSError(str)

    # No old file exists so just move it
    if not os.path.exists(filename):
        os.rename(filename + ".new", filename)
        print("...done creating new %s" % filename)
        return

    filename_new = filename + ".new"

    numDiffs = compare_except_lines(filename, filename_new, except_lines_list)
    if numDiffs > 0:
        # file has diffs so push .new file
        if dated_files_enable == True:
            timeTag = fileTimeTag(filename)
            if os.path.exists(filename + timeTag):
                os.remove(filename + timeTag)
            os.rename(filename, filename + ".old" + timeTag)
            print("...moved old file to {}.old{}".format(filename, timeTag))
        #
        if os.path.exists(filename):
            os.remove(filename)
        os.rename(filename_new, filename)
        print("...replaced old with new %s" % filename)
    else:
        # file does not have diffs
        if dated_files_enable == True:
            timeTag = fileTimeTag(filename)
            if os.path.exists(filename + ".new" + timeTag):
                os.remove(filename + ".new" + timeTag)
            os.rename(filename_new, filename + ".new" + timeTag)
            print("...new %s did not differ from old file." % filename)
            print("...keeping new file as {}.new{}".format(filename, timeTag))
        #
        else:
            os.remove(filename_new)
            print(
                "...new %s did not differ from old file - removing new file." % filename
            )


def DiffAndRename(filename, dated_files_enable=True):
    """
    Compare two files that are named 'file' and 'file.new'
    If the files differ by more than one line (the time stamp),
    then move 'file' to '.file.bu' and move 'file.new' to 'file'

    date_files_enable allows one to turn off the generation
    of the *.new.date and *.old.date files so CC will not
    go crazy.  LJR - 10 Aug. 2007.
    """
    if not os.path.exists(filename + ".new"):
        print("Cannot find %s.new" % filename)
        return

    if not os.path.exists(filename):
        os.rename(filename + ".new", filename)
        print("... done creating new %s" % filename)
        return

    old = open(filename).readlines()
    new = open(filename + ".new").readlines()

    # print "LEN OLD = %d " % len(old)
    # print "LEN NEW = %d " % len(new)

    filesDiffer = False
    numDiffs = 0
    if len(old) != len(new):
        filesDiffer = True
    else:
        # maybe they did not change
        for lineNum in range(0, len(old) - 1):
            oldLine = old[lineNum]
            newLine = new[lineNum]
            if oldLine != newLine:
                numDiffs += 1
                if numDiffs > 1:
                    filesDiffer = True
                    break

    if removeJunk:
        if filesDiffer:
            os.remove(filename)
            os.rename(filename + ".new", filename)
            print("... replaced old with new %s" % filename)
        else:
            os.remove(filename + ".new")
            print("... new %s did not differ from old file." % filename)
    else:
        if filesDiffer:
            if dated_files_enable == True:
                timeTag = fileTimeTag(filename)
                if os.path.exists(filename + timeTag):
                    os.remove(filename + timeTag)
                os.rename(filename, filename + ".old" + timeTag)
                print("... moved old file to {}.old{}".format(filename, timeTag))
            os.rename(filename + ".new", filename)
            print("... and replaced old with new %s" % filename)
        else:
            if dated_files_enable == True:
                timeTag = fileTimeTag(filename)
                if os.path.exists(filename + ".new" + timeTag):
                    os.remove(filename + ".new" + timeTag)
                os.rename(filename + ".new", filename + ".new" + timeTag)
                print("... new %s did not differ from old file." % filename)
                print("... keeping new file as {}.new{}".format(filename, timeTag))
            else:
                os.remove(filename + ".new")
                print("... removing new file")


def renameAsErroneous(fileName, script=None):
    """
    fileName argument represents the name of the target file, e.g.,
    mod_ac_evr_ids.h. If the generation of the new file (named
    mod_ac_evr_ids.h.new by convention in all actools scripts)
    has some problems, then rather than ever replacing the old
    file with the new flawed file, the new flawed file will be
    named with a time-stamp and '.error', for example:
    mod_ac_evr_ids.h.20071101T003932.errors
    """
    origfileName = fileName + ".new"
    timeTag = fileTimeTag(origfileName)
    errfileName = fileName + timeTag + ".errors"
    os.rename(origfileName, errfileName)
    if script:
        PRINT.error(
            "{}: this generated file is invalid: {}".format(script, errfileName)
        )
    else:
        PRINT.error("This generated file is invalid: %s" % errfileName)


def test_remove_files(pattern):
    filesInDir = os.listdir(".")
    filesMatchingPattern = fnmatch.filter(filesInDir, pattern)
    for fileName in filesMatchingPattern:
        print("removing %s" % fileName)
        os.remove(fileName)


def test_files_exist(pattern):
    filesInDir = os.listdir(".")
    filesMatchingPattern = fnmatch.filter(filesInDir, pattern)
    if 0 < len(filesMatchingPattern):
        return True
    return False


def test_files_matching_pattern(pattern):
    filesInDir = os.listdir(".")
    return fnmatch.filter(filesInDir, pattern)


if __name__ == "__main__":
    import fnmatch

    data = """
first line
second lineroo
third line
fourth line
last time here
"""
    test_remove_files("foo*")

    now = datetime.datetime.now()
    delta = datetime.timedelta(minutes=2)

    filename = "foo"
    print("Test 1:")
    print("expect 'Cannot find %s.new'" % filename)
    removeJunk = True
    DiffAndRename("foo")

    print()
    print("Test 2:")
    print("expect '... done creating new %s" % filename)
    new = open("foo.new", "w")
    new.write(now.ctime())
    new.write(data)
    new.close()
    removeJunk = True
    DiffAndRename("foo")
    if not os.path.exists("foo"):
        print("ERROR -- did not create foo")
    if test_files_exist("foo.new*"):
        print("ERROR -- did not rename foo.new")
    if test_files_exist("foo.old*"):
        print("ERROR -- no reason to create foo.old")

    print()
    print("Test 3:")
    print("expect '... new %s did not differ from old file." % filename)
    origstatinfo = os.stat("foo")
    new = open("foo.new", "w")
    now += delta
    new.write(now.ctime())
    new.write(data)
    new.close()
    removeJunk = True
    DiffAndRename("foo")
    if not os.path.exists("foo"):
        print("ERROR -- foo went away")
    if test_files_exist("foo.new*"):
        print("ERROR -- did not delete foo.new")
    if test_files_exist("foo.old*"):
        print("ERROR -- no reason to create foo.old")
    afterstatinfo = os.stat("foo")
    fileChanged = False
    for i in range(0, len(origstatinfo) - 1):
        if origstatinfo[i] != afterstatinfo[i]:
            print("ERROR -- file changed i=%d" % i)
            fileChanged = True

    print()
    print("Test 4:")
    print("expect '...  replaced old with new %s" % filename)
    new = open("foo.new", "w")
    now += delta
    new.write(now.ctime())
    new.write(data)
    new.write(data)
    new.close()
    removeJunk = True
    DiffAndRename("foo")
    if not os.path.exists("foo"):
        print("ERROR -- foo went away")
    if test_files_exist("foo.new*"):
        print("ERROR -- did not delete foo.new")
    if test_files_exist("foo.old*"):
        print("ERROR -- moved foo to foo.old")
    afterstatinfo = os.stat("foo")
    fileChanged = False
    for i in range(len(origstatinfo)):
        if origstatinfo[i] != afterstatinfo[i]:
            print("OK -- file changed i=%d" % i)
            fileChanged = True
    if not fileChanged:
        print("ERROR -- file did not change!")

    test_remove_files("foo*")

    print()
    print("Test 5:")
    print("expect 'Cannot find %s.new'" % filename)
    removeJunk = False
    DiffAndRename("foo")

    print()
    print("Test 6:")
    print("expect '... done creating new %s" % filename)
    new = open("foo.new", "w")
    new.write(now.ctime())
    new.write(data)
    new.close()
    removeJunk = False
    DiffAndRename("foo")
    if not os.path.exists("foo"):
        print("ERROR -- did not create foo")
    if not os.path.exists("foo"):
        print("ERROR -- did not rename foo.new")
    if test_files_exist("foo.new*"):
        print("ERROR -- should not rename to foo.new.timestamp")
    if test_files_exist("foo.old*"):
        print("ERROR -- no reason to create foo.old.timestamp")

    print()
    print("Test 7:")
    print("expect '... new foo did not differ from old file.")
    print("and    '... keeping new file as foo.new.yyyymmddThhmmss")
    origstatinfo = os.stat("foo")
    new = open("foo.new", "w")
    now += delta
    new.write(now.ctime())
    new.write(data)
    new.close()
    removeJunk = False
    DiffAndRename("foo")
    if not os.path.exists("foo"):
        print("ERROR -- foo went away")
    if os.path.exists("foo.new"):
        print("ERROR -- did not rename foo.new")
    if not test_files_exist("foo.new*"):
        print("ERROR -- did not keep foo.new.yyyymmddThhmmss")
    if test_files_exist("foo.old*"):
        print("ERROR -- should be no foo.old")
    afterstatinfo = os.stat("foo")
    fileChanged = False
    for i in range(0, len(origstatinfo) - 1):
        if origstatinfo[i] != afterstatinfo[i]:
            print("ERROR -- file changed i=%d" % i)
            fileChanged = True

    print()
    print("Test 8:")
    print("expect '... moved old file to %s.old.timestamp" % filename)
    print("and    '... and replaced old with new %s" % filename)
    new = open("foo.new", "w")
    now += delta
    new.write(now.ctime())
    new.write(data)
    new.write(data)
    new.close()
    removeJunk = False
    DiffAndRename("foo")
    if not os.path.exists("foo"):
        print("ERROR -- foo went away")
    if os.path.exists("foo.new"):
        print("ERROR -- did not delete foo.new")
    if not test_files_exist("foo.old*"):
        print("ERROR -- should be foo.old.timestamp")
    afterstatinfo = os.stat("foo")
    fileChanged = False
    for i in range(0, len(origstatinfo) - 1):
        if origstatinfo[i] != afterstatinfo[i]:
            print("OK -- file changed i=%d" % i)
            fileChanged = True
    if not fileChanged:
        print("ERROR -- file did not change!")

    print()
    print("Test 9:")
    print("expect '... moved old file to %s.old" % filename)
    print("and    '... and replaced old with new %s" % filename)
    files = test_files_matching_pattern("foo.old*")
    if 1 != len(files):
        print("ERROR -- too many old files.")
    before_stat_foo_old = os.stat(files[0])
    before_stat_foo = os.stat("foo")
    new = open("foo.new", "w")
    now += delta
    new.write(now.ctime())
    new.write(data)
    new.write(data)
    new.write(data)
    new.close()
    before_stat_foo_new = os.stat("foo.new")
    removeJunk = False
    DiffAndRename("foo")
    if not os.path.exists("foo"):
        print("ERROR -- foo went away")
    if os.path.exists("foo.new"):
        print("ERROR -- did not delete foo.new")
    if not test_files_exist("foo.old*"):
        print("ERROR -- should be foo.old.timestamp")
    files = test_files_matching_pattern("foo.old*")
    if len(files) < 1:
        print("ERROR -- should be at least one, probably two, foo.old*")
    after_stat_foo_old = os.stat(files[0])
    after_stat_foo = os.stat("foo")

    fileChanged = False
    for i in range(0, len(before_stat_foo) - 1):
        if before_stat_foo[i] != after_stat_foo_old[i]:
            print("ERROR -- foo.old should be old foo -- changed i=%d" % i)
            fileChanged = True
    if fileChanged:
        print("ERROR -- foo.old should be old foo!")
    fileChanged = False
    for i in range(0, len(before_stat_foo) - 1):
        if before_stat_foo_new[i] != after_stat_foo[i]:
            print("ERROR -- foo should be old foo.new -- changed i=%d" % i)
            fileChanged = True
    if fileChanged:
        print("ERROR -- foo should be old foo.new!")

    test_remove_files("foo*")
