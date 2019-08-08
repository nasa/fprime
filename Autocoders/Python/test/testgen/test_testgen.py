"""
test_testgen.py:

Checks that the testgen tool is properly generating its test cpp/hpp.

@author jishii
"""

import os
import sys

import subprocess
from subprocess import CalledProcessError
import pexpect
from pexpect import TIMEOUT, EOF
from optparse import OptionParser

import shutil
import tempfile
import filecmp
import logging
import time

# Version label for now
class Version:
    id      = "0.1"
    comment = "Initial prototype"
VERSION = Version()

def add_tester_section(filename, section_flag, section_type):
    """
    Append Tester.cpp/hpp with the appropriate test cases
    for the math component
    """
    with open(filename, "r+") as fileopen:
        lines = fileopen.readlines()
        
        # Don't duplicate the test cases
        if "testAddCommand(void)" in str(lines):
            return
        
        # Starting at line 0, rewrite Tester.cpp/hpp with test cases
        fileopen.seek(0)
        skip_lines = 0
        for line in lines:
            if not line:
                break
            
            # Skip_lines allows user to print out test cases at an
            # offset from method argument "section_flag"
            if skip_lines <= 0:
                if section_flag in line:
                    if section_type == "TESTER_METHODS":
                        skip_lines = 2 # Skip to todo method init
                    if section_type == "TESTER_HEADERS":
                        skip_lines = 2 # Skip to todo method def
                    fileopen.write(line)
                else:
                    fileopen.write(line)
            else:
                skip_lines = skip_lines - 1
                
                if skip_lines == 0:
                    write_tester_lines(fileopen, section_type)
                
                fileopen.write(line)

        fileopen.truncate()

def write_tester_lines(fileopen, section_type):
    """
    Write test cases inside "fileopen" at whatever location
    "fileopen" is currently pointing to
    """
    testgen_dir = "Autocoders" + os.sep + "Python" + os.sep + "test" + os.sep + "testgen" + os.sep
    if section_type == "TESTER_METHODS":
        # Test case method initializations found in local file
        with open(testdir + "__tester_handcoded_methods.txt", "r") as methodstub:
            for line in methodstub:
                fileopen.write(line)
    if section_type == "TESTER_HEADERS":
        fileopen.write("\t\t//! Test operation command\n")
        fileopen.write("\t\t//!\n")
        fileopen.write("\t\tvoid testAddCommand(void);\n")
        fileopen.write("\t\tvoid testSubCommand(void);\n")
        fileopen.write("\t\tvoid testMultCommand(void);\n")
        fileopen.write("\t\tvoid testDivCommand(void);\n")

def file_diff(file1, file2):
    """
    Returns set of line numbers from file1 which differ from file2
    """
    diff_lines = set()
    with open(file1, "r") as file1open:
        with open(file2, "r") as file2open:
            count = 0
            # Compare line by line until a file hits EOF
            while(1):
                line1 = file1open.readline()
                line2 = file2open.readline()
                if not line1 or not line2:
                    break
                elif not line1 == line2:
                    diff_lines.add(count)
                count += 1

    # Warn user if one file is longer than the other
    if file_len(file1) < file_len(file2):
        print("WARNING: " + file2 + " has more lines than " + file1)
    elif file_len(file1) < file_len(file2):
        print("WARNING: " + file1 + " has more lines than " + file2)

    return diff_lines

def file_len(fname):
    """
    Helper method to get number of lines in file
    """
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
        # Skip_docstring removes all lines in between two """ characters
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
    if ".py" in filename:
        filename = "DefaultDict/serializable/{}".format(filename)
    
    remove_headers(filename)

    if not (filecmp.cmp(filename, testdir + "templates" + os.sep + "{}".format(filename).replace(".", "_") + ".txt")):
        print("WARNING: {} generated incorrectly according to Autocoders" + os.sep + "Python" + os.sep + "test" + os.sep + "enum_xml" + os.sep + "templates" + os.sep + "{}".format(filename, filename.replace(".", "_") + ".txt"))
        diff_lines = file_diff(filename, testdir + "templates" + os.sep + "{}".format(filename).replace(".", "_") + ".txt")
        print("WARNING: the following lines from " + filename + " differ from the template: " + str(diff_lines))
    else:
        print("{} is consistent with expected template".format(filename))

def test_testgen():
    """
    Tests that tests are being generated correctly
    """
    global testdir

    try:
        # cd into test directory to find test files (code/test/dictgen can only find files this way)
        curdir = os.getcwd()
        testdir = os.environ['BUILD_ROOT'] + os.sep + "Autocoders" + os.sep
        testdir = testdir + "Python" + os.sep + "test" + os.sep + "testgen" + os.sep
        testutdir = testdir + "test" + os.sep + "ut"
        os.chdir(testdir)
        
        bindir = os.environ['BUILD_ROOT'] + os.sep + "Autocoders" + os.sep + "Python" + os.sep + "bin" + os.sep
        
        # Autocode component and port
        pport1 = pexpect.spawn("python " + bindir + "codegen.py -v " + testdir + "MathOpPortAi.xml")
        pport1.expect("(?=.*Generating code filename: MathOpPortAc.cpp, using default XML filename prefix...)(?=.*Generating code filename: MathOpPortAc.hpp, using default XML filename prefix...)(?!.*ERROR).*")
        pport2 = pexpect.spawn("python " + bindir + "codegen.py -v " + testdir + "MathResultPortAi.xml")
        pport2.expect("(?=.*Generating code filename: MathResultPortAc.cpp, using default XML filename prefix...)(?=.*Generating code filename: MathResultPortAc.hpp, using default XML filename prefix...)(?!.*ERROR).*")
        pcomp = pexpect.spawn("python " + bindir + "codegen.py -v " + testdir + "MathSenderComponentAi.xml")
        pcomp.expect("(?=.*MathSender)(?=.*MathOp)(?=.*MathResult)(?!.*ERROR).*")
        
        os.chdir(testutdir)
        
        print(os.getcwd())
        
        # Autocode tests
        p = pexpect.spawn("python " + bindir + "testgen.py -v -f " + testdir + "MathSenderComponentAi.xml")

        p.expect("(?=.*Generated test files)(?=.*MathSenderComponentAi.xml)(?=.*Generated TesterBase.hpp)(?=.*Generated TesterBase.cpp)(?=.*Generated GTestBase.hpp)(?=.*Generated GTestBase.cpp)(?=.*Generated Tester.cpp)(?=.*Generated Tester.hpp)(?!.*ERROR).*", timeout=5)

        print("Autocoded TestComponent")
        
        time.sleep(1)

        add_tester_section("Tester.cpp", "// Tests", "TESTER_METHODS")
        add_tester_section("Tester.hpp", "//! To do", "TESTER_HEADERS")
        
        time.sleep(1)
        
        # Run testgen again to generate main with test cases
        p2 = pexpect.spawn("python " + bindir + "testgen.py -v -m " + testdir + "MathSenderComponentAi.xml")
        
        p2.expect("(?=.*MathSenderComponentAi.xml)(?=.*Generated TesterBase.hpp)(?=.*Generated TesterBase.cpp)(?=.*Generated GTestBase.hpp)(?=.*Generated GTestBase.cpp)(?=.*Generated TestMain.cpp)(?!.*ERROR).*", timeout=5)
        
        # Test whether all generated files match expected
        compare_genfile("Tester.cpp")
        compare_genfile("Tester.hpp")
        compare_genfile("TesterBase.cpp")
        compare_genfile("TesterBase.hpp")
        compare_genfile("GTestBase.cpp")
        compare_genfile("GTestBase.hpp")
        compare_genfile("TestMain.cpp")
            
        
        try:
            rootdir = tempfile.mkdtemp() + os.sep
            os.chdir(rootdir)
            
            # Create build dir for unit tests if it doesn't exist
            if not os.path.exists(rootdir):
                os.mkdir(rootdir)

            builddir = rootdir
            os.chdir(builddir)
            
            # Run cmake to configure testgen test
            if not os.path.exists(builddir + "F-Prime" + os.sep + "Autocoders" + os.sep + "Python" + os.sep + "test" + os.sep + "testgen" + os.sep + "Makefile"):
                pcmake = pexpect.spawn("cmake {} -DCMAKE_BUILD_TYPE=TESTING".format(os.environ["BUILD_ROOT"]))
                pcmake.expect("(?=.*Configuring done)(?=.*Generating done)(?=.*Build files have been written)")
                print("Successfully ran cmake for testgen test")
            
            # Build testgen ut
            pbuild = pexpect.spawn("make Autocoders_Python_test_testgen_ut_exe -j32")
            pbuild.expect("(?=.*Built target Autocoders_Python_test_testgen_ut_exe)")
            
            print("Built testgen unit test")

            utdir = builddir + "bin" + os.sep + os.uname()[0] + os.sep + "Autocoders_Python_test_testgen_ut_exe"
            
            # Run ut
            ptestrun = pexpect.spawn(utdir)
            
            print(ptestrun.read().decode("ascii"))
            
            ptestrun.expect("(?!.*FAILED)(?!.*ASSERT).*");
            
            print("Successfully ran testgen unit test")
            
            # Remove generated files
            os.chdir(testdir)
            if os.path.exists("MathOpPortAc.cpp"):
                os.remove("MathOpPortAc.cpp")
            if os.path.exists("MathOpPortAc.hpp"):
                os.remove("MathOpPortAc.hpp")
            if os.path.exists("MathResultPortAc.cpp"):
                os.remove("MathResultPortAc.cpp")
            if os.path.exists("MathResultPortAc.hpp"):
                os.remove("MathResultPortAc.hpp")
            if os.path.exists("MathSenderComponentAc.cpp"):
                os.remove("MathSenderComponentAc.cpp")
            if os.path.exists("MathSenderComponentAc.hpp"):
                os.remove("MathSenderComponentAc.hpp")
            
            ## If there was no timeout the pexpect test passed
            assert True
        finally:
            shutil.rmtree(rootdir)
                
            os.chdir(curdir)
        
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
