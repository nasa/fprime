"""
test_enum.py:

Checks that the testgen tool is properly generating its test cpp/hpp.

@author jishii
"""

import os

import subprocess
from subprocess import CalledProcessError
import pexpect
from pexpect import TIMEOUT, EOF

import filecmp
import logging
import time

def file_diff(file1, file2):
    """
    Returns set of line numbers from file1 which differ from file2
    """
    diff_lines = set()
    with open(file1, "r") as file1open:
        with open(file2, "r") as file2open:
            count = 0
            while(1):
                line1 = file1open.readline()
                line2 = file2open.readline()
                if not line1 or not line2:
                    break
                elif not line1 == line2:
                    diff_lines.add(count)
                count += 1
    return diff_lines


    if file_len(file1) < file_len(file2):
        print("WARNING: " + file2 + " has more lines than " + file1)
    elif file_len(file1) < file_len(file2):
        print("WARNING: " + file1 + " has more lines than " + file2)

def file_len(fname):
    with open(fname) as f:
        for i, l in enumerate(f):
            pass
    return i + 1

def remove_headers(filename):
    """
    Remove header comment so that date doesn't
    mess up the file comparison
    """
    with open(filename, "r+") as f:
        lines = f.readlines()
        f.seek(0)
        num = 0
        for line in lines:
            if not (num == 0 and ('*' in line or '//' in line)):
                # Don't want to print empty first line
                if num != 0 or line.strip():
                    f.write(line)
                    num += 1
        
        f.truncate()

def compare_genfile(filename):
    """
    Compares genfile with expected genfile
    """
    if ".py" in filename:
        filename = "DefaultDict/serializable/{}".format(filename)
    
    remove_headers(filename)

    if not (filecmp.cmp(filename,"templates/{}".format(filename))):
        print("WARNING: {} generated incorrectly according to Autocoders/Python/test/enum_xml/templates/{}".format(filename, filename))
        diff_lines = file_diff(filename, "templates/{}".format(filename))
        print("WARNING: the following lines from " + filename + " differ from the template: " + str(diff_lines))
    else:
        print("{} is consistent with expected template".format(filename))

def test_testgen():
    """
    Tests that tests are being generated correctly
    """
    try:
        
        ## Spawn executable
        p = pexpect.spawn("python ../../bin/testgen.py -v TestComponentAi.xml")
        
        p.expect("(?=.*Generated test files for TestComponentAi.xml)(?=.*Generated TesterBase.hpp)(?=.*Generated TesterBase.cpp)(?=.*Generated GTestBase.hpp)(?=.*Generated GTestBase.cpp)(?=.*Generated Tester.hpp)(?=.*Generated Tester.cpp)(?!.*ERROR).*", timeout=5)
        
        time.sleep(3)
        
        # Test whether all generated files match expected
        compare_genfile("Tester.cpp")
        compare_genfile("Tester.hpp")
        compare_genfile("TesterBase.cpp")
        compare_genfile("TesterBase.hpp")
        compare_genfile("GTestBase.cpp")
        compare_genfile("GTestBase.hpp")
        
        ## If there was no timeout the pexpect test passed
        assert True
        
    ## A timeout occurs when pexpect cannot match the executable
    ## output with the designated expectation. In this case the
    ## key expectation is p.expect(expect_string, timeout=3)
    ## which tests what the method name describes
    except TIMEOUT as e:
        print("Timeout Error. Expected Value not returned.")
#        print ("-------Program Output-------")
#        print (ptestrun.before)
        print ("-------Expected Output-------")
        print (e.get_trace())
        assert False
    except EOF as e:
        print("EOF Error. Pexpect did not find expected output in program output.")
#        print ("-------Program Output-------")
#        print (ptestrun.before)
        assert False
