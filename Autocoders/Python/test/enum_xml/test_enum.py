"""
test_enum.py:

Checks that the enum xml is being correctly autocoded and imported into
components, porttypes, and serializables.

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
import tempfile
import shutil

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
    if ".py" in filename:
        filename = "DefaultDict/serializable/{}".format(filename)

    remove_headers(filename)

    if ".cpp" in filename:
        filename2 = filename.split(".")[0] + "_cpp.txt"
    elif ".hpp" in filename:
        filename2 = filename.split(".")[0] + "_hpp.txt"
    elif ".py" in filename:
        filename2 = filename
    else:
        print("Unrecognized file name extention for file {}".format(filename))
        
    if not (filecmp.cmp(filename,"templates/{}".format(filename2))):
        print("WARNING: {} generated incorrectly according to Autocoders/Python/test/enum_xml/templates/{}".format(filename, filename2))
        diff_lines = file_diff(filename, "templates/{}".format(filename2))
        print("WARNING: the following lines from " + filename + " differ from the template: " + str(diff_lines))
    else:
        print("{} is consistent with expected template".format(filename))

def test_enum():
    """
    Tests that enum xml is being generated correctly
    This test requires PYTONPATH, BUILD_ROOT, and FPRIME_CORE_DIR build vars to be set

    @TODO: This test and others need the cmake setup instructions.
    """
    try:
        # Check required environmental variables are set
        if "PYTHONPATH" in os.environ.keys():
            print("\nFound PYTHONPATH = {}".format(os.environ['PYTHONPATH']))
        else:
            assert False
        
        if "BUILD_ROOT" in os.environ.keys():
            print("Found BUILD_ROOT = {}".format(os.environ['BUILD_ROOT']))
        else:
            assert False
        
        if "FPRIME_CORE_DIR" in os.environ.keys():
            print("Found FPRIME_CORE_DIR = {}".format(os.environ['FPRIME_CORE_DIR']))
        else:
            assert False
        

        curdir = os.getcwd()
        
        # Need to cd into test directory in order to add test to CMakeList
        testdir = os.sep + os.environ['BUILD_ROOT'] + os.sep + "Autocoders" + os.sep
        testdir = testdir + os.sep + "Python" + os.sep + "test" + os.sep
        os.chdir(testdir)
        
        # Need to cd into test directory in order to run codegen, will return after completion
        enumtestdir = os.sep + os.environ['BUILD_ROOT'] + os.sep + "Autocoders" + os.sep
        enumtestdir = enumtestdir + os.sep + "Python" + os.sep + "test" + os.sep + "enum_xml"
        os.chdir(enumtestdir)
        
        expect_step = "enum and serializable"
        
        ## Spawn executable
        cmd = "python " + os.sep + os.environ['BUILD_ROOT'] + os.sep + "Autocoders" + os.sep
        cmd = cmd + os.sep + "Python" + os.sep + "bin" + os.sep + "codegen.py Enum1EnumAi.xml"
        penum = pexpect.spawn(cmd)
        penum.expect("(?=.*Enum1EnumAi.xml)(?!.*ERROR).*", timeout=5)
        print("\nEnum XML autocoded")
        
        cmd = "python " + os.sep + os.environ['BUILD_ROOT'] + os.sep + "Autocoders" + os.sep
        cmd = cmd + os.sep + "Python" + os.sep + "bin" + os.sep + "codegen.py Serial1SerializableAi.xml"
        print(cmd)
        pserial = pexpect.spawn(cmd)
        pserial.expect("(?=.*Serial1SerializableAc.cpp)(?=.*Serial1SerializableAc.hpp)(?!.*ERROR).*", timeout=5)
        print("Serializable XML autocoded")
        
        expect_step = "port and component"
        
        cmd = "python " + os.sep + os.environ['BUILD_ROOT'] + os.sep + "Autocoders" + os.sep
        cmd = cmd + os.sep + "Python" + os.sep + "bin" + os.sep + "codegen.py Port1PortAi.xml"
        pport = pexpect.spawn(cmd)
        pport.expect("(?=.*Port1PortAc.cpp)(?=.*Port1PortAc.hpp)(?!.*ERROR).*", timeout=5)
        print("Port XML autocoded")

        cmd = "python " + os.sep + os.environ['BUILD_ROOT'] + os.sep + "Autocoders" + os.sep
        cmd = cmd + os.sep + "Python" + os.sep + "bin" + os.sep + "codegen.py Component1ComponentAi.xml"
        pcomp = pexpect.spawn(cmd)
        pcomp.expect("(?=.*Component1)(?=.*Port1)(?=.*Serial1)(?!.*ERROR).*", timeout=5)
        print("Component XML autocoded\n")
        
        expect_step = "genfile"
        
        # Give time to generate files
        time.sleep(3)

        # Test whether all generated files match expected
        compare_genfile("Enum1EnumAc.cpp")
        compare_genfile("Enum1EnumAc.hpp")
        compare_genfile("Serial1SerializableAc.cpp")
        compare_genfile("Serial1SerializableAc.hpp")
        compare_genfile("Port1PortAc.cpp")
        compare_genfile("Port1PortAc.hpp")
        compare_genfile("Component1ComponentAc.cpp")
        compare_genfile("Component1ComponentAc.hpp")
        compare_genfile("Enum1.py")
        
        expect_step = "cmake"
        try:
            builddir = tempfile.mkdtemp() + os.sep
            os.chdir(builddir)

            pcmake = pexpect.spawn("cmake {} -DCMAKE_BUILD_TYPE=TESTING".format(os.environ["BUILD_ROOT"]))
            pcmake.expect("(?=.*Configuring done)(?=.*Generating done)(?=.*Build files have been written)")
            print("Successfully ran cmake for enum xml test")

            # Build ut
            pbuild = pexpect.spawn("make Autocoders_Python_test_enum_xml_ut_exe -j32")
            pbuild.expect("(?=.*Built target Autocoders_Python_test_enum_xml_ut_exe)")
            
            print("\nRunning Test Cases...")
            
            ## Runs enum xml test and communicate through standard output to
            ## complete all test cases. Both test cases are creating 2 enums
            ## and a serializable, and then they invoke the ports and check
            ## serialization
            enum_dict = {1: -1952875139, 2: 2, 3: 2000999333, 4: 21, 5: -8324876}
            tc_data = [1, 2, 1, 2, 1, 2]
            enum1_data = [1, 2, 4, 3, 2, 5]
            enum2_data = [5, 4, 2, 3, 3, 1]
            enum1_literal_data = [-1952875139, 2, 2000999333, 21, -8324876, 3]
            enum2_literal_data = [-8324876, 21, 2000999333, 2, -1952875139, 2]
            serial1_data = [0, 10, 67, 4837, 82739845, 3434534]
            serial2_data = [22839745, 3453, 2, 1, 9808973, 99]
            cmd = builddir
            cmd = cmd + "bin" + os.sep + os.uname()[0] + os.sep + "Autocoders_Python_test_enum_xml_ut_exe"
            ptestrun = pexpect.spawn(cmd)
            
            ptestrun.expect(".*run or q to quit: ", timeout=3)
            ptestrun.sendline("e")
            for i in range(0, 6):
                ptestrun.expect(r".*Enter test case \(1-2\): ", timeout=3)
                ptestrun.sendline(str(tc_data[i]))
                print("Running test case {}".format(str(tc_data[i])))
                
                # For half of the tests use integer literals and half use the actual enum
                # to define the enum (21 vs Example::Enum1.Item3)
                if (tc_data[i] == 1):
                    print("Using enum1 with integer literal value {}, enum2 with value {}, serializable with arg1 value {}, serializable with arg2 value {}".format(str(enum1_literal_data[i]), str(enum2_literal_data[i]), str(serial1_data[i]), str(serial2_data[i])))
                    
                    ptestrun.expect(r".*first enum item number.*", timeout=3)
                    ptestrun.sendline(str(enum1_literal_data[i]))
                    ptestrun.expect(r".*second enum item number.*", timeout=3)
                    ptestrun.sendline(str(enum2_literal_data[i]))
                else:
                    print("Using enum1 with enum object value {}, enum2 with value {}, serializable with arg1 value {}, serializable with arg2 value {}".format(str(enum1_data[i]), str(enum2_data[i]), str(serial1_data[i]), str(serial2_data[i])))
                    
                    ptestrun.expect(r".*first enum item number.*", timeout=3)
                    ptestrun.sendline(str(enum1_data[i]))
                    ptestrun.expect(r".*second enum item number.*", timeout=3)
                    ptestrun.sendline(str(enum2_data[i]))
                
                ptestrun.expect(r".*serializable arg1: ", timeout=3)
                ptestrun.sendline(str(serial1_data[i]))
                ptestrun.expect(r".*serializable arg2: ", timeout=3)
                ptestrun.sendline(str(serial2_data[i]))

                # Check serialize tests
                expect_step = "cmake_serialize"
                ptestrun.expect(r".*Serialize enums \(y\/n\):.*", timeout=3)
                ptestrun.sendline("y")

                # Check deserialize tests
                expect_step = "cmake_deserialize"
                ptestrun.expect(r"(?=.*Serialized enum1)(?=.*Serialized enum2)(?=.*Deserialize enums \(y\/n\):).*", timeout=3)
                ptestrun.sendline("y")

                # Check enums == saved version pre-serialize
                expect_step = "cmake_checksave"
                ptestrun.expect(r"(?=.*Deserialized enum1)(?=.*Deserialized enum2)(?=.*Run enum check \(y\/n\):).*", timeout=3)
                ptestrun.sendline("y")

                # Check serializable with enum arg
                expect_step = "cmake_serializable"
                ptestrun.expect(r"(?=.*Successful enum1 check)(?=.*Successful enum2 check)(?=.*Create serializable \(y\):).*", timeout=3)
                ptestrun.sendline("y")

                # Invoke ports
                expect_step = "cmake_invoke"
                if tc_data[i] == 1:
                    ptestrun.expect(r"(?=.*Created serializable with enum arg)(?=.*enum should be used to invoke inst1 \(1-2\))", timeout = 3)
                elif tc_data[i] == 2:
                    ptestrun.expect(r"(?=.*Created serializable with enum arg)(?=.*enum should be used to invoke inst1 \(1-2\))", timeout = 3)
                ptestrun.sendline(str((i % 2) + 1)) # 1 or 2
                # Prompt for inst2 and check inst1 invoke
                if tc_data[i] == 1:
                    ptestrun.expect(r"(?=.*enum should be used to invoke inst2 \(1-2\))(?=.*inst1 Invoked ExEnumIn_handler\(0, {}, {}, {}, {}\))(?=.*inst2 Invoked EnumIn_handler\(0, {}, {}, {}, {}\)).*".format(enum1_literal_data[i] if (i % 2) + 1 == 1 else enum2_literal_data[i], serial1_data[i], serial2_data[i], enum2_literal_data[i], enum1_literal_data[i] if (i % 2) + 1 == 1 else enum2_literal_data[i], serial1_data[i], serial2_data[i], enum2_literal_data[i]), timeout=3)
                elif tc_data[i] == 2:
                    ptestrun.expect(r"(?=.*enum should be used to invoke inst2 \(1-2\))(?=.*inst1 Invoked EnumIn_handler\(0, {}, {}, {}, {}\)).*".format(enum_dict[enum1_data[i]] if (i % 2) + 1 == 1 else enum_dict[enum2_data[i]], serial1_data[i], serial2_data[i], enum_dict[enum2_data[i]], enum_dict[enum1_data[i]] if (i % 2) + 1 == 1 else enum_dict[enum2_data[i]], serial1_data[i], serial2_data[i], enum_dict[enum2_data[i]]), timeout=3)
                ptestrun.sendline(str(((i + 1) % 2) + 1)) # 1 or 2

                # Ensure that inst2 properly invoked then loop back or quit
                if tc_data[i] == 1:
                    ptestrun.expect(r"(?=.*run or q to quit: )(?=.*inst2 Invoked ExEnumIn_handler\(0, {}, {}, {}, {}\))(?=.*inst1 Invoked EnumIn_handler\(0, {}, {}, {}, {}\)).*".format(enum1_literal_data[i] if ((i + 1) % 2) + 1 == 1 else enum2_literal_data[i], serial1_data[i], serial2_data[i], enum2_literal_data[i], enum1_literal_data[i] if ((i + 1) % 2) + 1 == 1 else enum2_literal_data[i], serial1_data[i], serial2_data[i], enum2_literal_data[i]), timeout=3)
                else:
                    ptestrun.expect(r"(?=.*run or q to quit: )(?=.*inst2 Invoked EnumIn_handler\(0, {}, {}, {}, {}\)).*".format(enum_dict[enum1_data[i]] if ((i + 1) % 2) + 1 == 1 else enum_dict[enum2_data[i]], serial1_data[i], serial2_data[i], enum_dict[enum2_data[i]], enum_dict[enum1_data[i]] if ((i + 1) % 2) + 1 == 1 else enum_dict[enum2_data[i]], serial1_data[i], serial2_data[i], enum_dict[enum2_data[i]]), timeout=3)
                if i == 5: # quit on 5
                    ptestrun.sendline("q")
                else:
                    ptestrun.sendline("e")

            print("Finished test case {}".format(str(tc_data[i])))
            
            ptestrun.expect(r".*Completed.*", timeout=3)
            print("Finished running test cases, enum xml test passed")
            
            # Remove autogenerated files
            os.chdir(enumtestdir)
            shutil.rmtree("DefaultDict")
            
            ## If there was no timeout the pexpect test passed
            assert True

        finally:
            shutil.rmtree(builddir)
                
            os.chdir(curdir)
        
    ## A timeout occurs when pexpect cannot match the executable
    ## output with the designated expectation. In this case the
    ## key expectation is p.expect(expect_string, timeout=3)
    ## which tests what the method name describes
    ## BOTH TIMEOUT AND EOF ARE GENERIC PEXPECT ERRORS AND
    ## PROBABLY SHOW THAT SOMETHING IS WRONG WITH THE TEST ITSELF.
    ## IF THEY ARE CALLED, USER SHOULD CHECK IF TEST NEEDS UPDATING.
    except TIMEOUT as e:
        print("Timeout Error. Expected Value not returned.")
#        print ("-------Program Output-------")
#        print (ptestrun.before)
        print ("-------Expected Output-------")
        print (e.get_trace())
        assert False
    except EOF as e:
        if expect_step == "enum":
            print("EOF Error. Pattern was not matched. Enum1EnumAi.xml generated errors when run in codegen.py")
        elif expect_step == "serializable":
            print("EOF Error. Pattern was not matched. Serial1SerializableAi.xml generated errors when run in codegen.py")
        elif expect_step == "port":
            print("EOF Error. Pattern was not matched. Port1PortAi.xml generated errors when run in codegen.py")
        elif expect_step == "component":
            print("EOF Error. Pattern was not matched. Component1ComponentAi.xml generated errors when run in codegen.py")

        # These states all correspond with equivalent of FW_ASSERTS in cpp when testing serialize / deserialize / port invocation
        elif expect_step == "cmake":
            print("EOF Error. Test did not build successfully or input was not performed correctly.")
        elif expect_step == "cmake_serialize":
            print("EOF Error. main.cpp asserted on the serialization")
        elif expect_step == "cmake_deserialize":
            print("EOF Error. main.cpp asserted on the deserialization")
        elif expect_step == "cmake_checksave":
            print("EOF Error. enums are not what was expected after serialize/deserialize")
        elif expect_step == "cmake_serializable":
            print("EOF Error. main.cpp asserted when putting enum value into serializable")
        elif expect_step == "cmake_invoke":
            print("EOF Error. main.cpp asserted during port invokes")

        
#        print ("-------Program Output-------")
#        print (ptestrun.before)
        assert False
