"""
test_enum.py:

Checks that the dictgen tools are properly generating python/xml dicts and.

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
    mess up the file comparison - also removes all docstrings
    """
    with open(filename, "r+") as f:
        lines = f.readlines()
        f.seek(0)
        num = 0
        skip_docstring = False
        for line in lines:
            if not skip_docstring:
                if not (num == 0 and ('*' in line or '//' in line or "'''" in line or '"""' in line)):
                    # Don't want to print empty first line
                    if num != 0 or line.strip():
                        f.write(line)
                        num += 1
                            
            if "'''" in line or '"""' in line:
                skip_docstring = not skip_docstring
        
        f.truncate()

def compare_genfile(filename):
    """
    Compares genfile with expected genfile
    """
    remove_headers(filename)

    if not (filecmp.cmp(filename,"templates/{}".format(filename))):
        print("WARNING: {} generated incorrectly according to Autocoders/Python/test/enum_xml/templates/{}".format(filename, filename + ".txt"))
        diff_lines = file_diff(filename, "templates/{}".format(filename) + ".txt")
        print("WARNING: the following lines from " + filename + " differ from the template: " + str(diff_lines))
    else:
        print("{} is consistent with expected template".format(filename))

def test_dictgen():
    """
    Tests that tests are being generated correctly
    """
    try:
        
        # cd into test directory to find test files (code/test/dictgen can only find files this way)
        curdir = os.getcwd()
        testdir = os.sep + os.environ['BUILD_ROOT'] + os.sep + "Autocoders" + os.sep
        testdir = testdir + "Python" + os.sep + "test" + os.sep + "dictgen"
        os.chdir(testdir)
        
        bindir = os.sep + os.environ['BUILD_ROOT'] + os.sep + "Autocoders" + os.sep + "Python" + os.sep + "bin" + os.sep
        
        ## Spawn executable
        p_pymod = pexpect.spawn("python " + bindir + "pymod_dictgen.py -v TestTopologyAppAi.xml")
        
        p_pymod.expect("(?=.*Generated component dicts for DictGen::TestComponent)(?=.*Generated tests for topology TestTopology)(?!.*ERROR).*", timeout=5)
        
        print("Autocoded TestTopology using pymod dictgen tool")
        
        p_gds = pexpect.spawn("python " + bindir + "gds_dictgen.py -v TestTopologyAppAi.xml")
        
        p_gds.expect("(?=.*Generated XML dictionary TestTopologyAppDictionary.xml)(?!.*ERROR).*", timeout=5)
        
        print("Autocoded TestTopology using gds dictgen tool")
        
        time.sleep(3)
        
        # Test whether all generated files match expected
        compare_genfile("TestTopologyAppDictionary.xml")
        compare_genfile("channels/Inst1_Test_Tlm0.py")
        compare_genfile("channels/Inst2_Test_Tlm0.py")
        compare_genfile("commands/Inst1_TEST_CMD1.py")
        compare_genfile("commands/Inst1_Test_Cmd0.py")
        compare_genfile("commands/Inst1_Test_Cmd1.py")
        compare_genfile("commands/Inst1_Test_Cmd2.py")
        compare_genfile("commands/Inst2_TEST_CMD_1.py")
        compare_genfile("commands/Inst2_Test_Cmd0.py")
        compare_genfile("commands/Inst2_Test_Cmd1.py")
        compare_genfile("commands/Inst2_Test_Cmd2.py")
        compare_genfile("events/Inst1_Test_Evr0.py")
        compare_genfile("events/Inst1_Test_Evr1.py")
        compare_genfile("events/Inst1_Test_Evr2.py")
        compare_genfile("events/Inst2_Test_Evr0.py")
        compare_genfile("events/Inst2_Test_Evr1.py")
        compare_genfile("events/Inst2_Test_Evr2.py")
        
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
