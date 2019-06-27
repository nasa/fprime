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
    print(filename)
    
    remove_headers(filename)

    if not (filecmp.cmp(filename,"templates/{}".format(filename))):
        print("ERROR: {} generated incorrectly according to Autocoders/Python/test/enum_xml/templates/{}".format(filename, filename))
        assert False

def test_enum():
    """
    Tests that enum xml is being generated correctly
    """
    try:
        ## Spawn executable
        p = pexpect.spawn("python ../../bin/codegen.py Enum1EnumAi.xml Serial1SerializableAi.xml")
        
        # Expect all of the filenames + expect output does not contain ERROR
        p.expect("(?=.*Serial1SerializableAi.xml)(?=.*Enum1EnumAi.xml).*", timeout=5)
        
        # pexpect doesn't like to generate 8 files in 1 command so I split the work in half
        p2 = pexpect.spawn("python ../../bin/codegen.py Port1PortAi.xml Component1ComponentAi.xml")
        p2.expect("(?=.*Port1PortAi.xml)(?=.*Component1ComponentAi.xml)(?!.*ERROR).*", timeout=5)
        
        # Give time to generate files
        time.sleep(3)

        # Test whether all generated files match expected
        compare_genfile('Enum1EnumAc.cpp')
        compare_genfile('Enum1EnumAc.hpp')
        compare_genfile('Serial1SerializableAc.cpp')
        compare_genfile('Serial1SerializableAc.hpp')
        compare_genfile('Port1PortAc.cpp')
        compare_genfile('Port1PortAc.hpp')
        compare_genfile('Component1ComponentAc.cpp')
        compare_genfile('Component1ComponentAc.hpp')
        
#        p3 = pexpect.spawn("cd ../../../../build_test/ && cmake .. && make check -j 32", timeout = 15)

        logging.debug('No timeout. Asserting test as true.')
        ## If there was no timeout the pexpect test passed
        assert True
        
        ## A timeout occurs when pexpect cannot match the executable
        ## output with the designated expectation. In this case the
        ## key expectation is p.expect(expect_string, timeout=3)
        ## which tests what the method name describes
    except TIMEOUT as e:
        print("Timeout Error. Expected Value not returned.")
        print ("-------Program Output-------")
        print (p.before)
        print ("-------Expected Output-------")
        print (e.get_trace())
        assert False
    except EOF as e:
        print("EOF Error. Pattern was not matched. Either codegen.py output contains error, or missing file(s) Serial1SerializableAi.xml, Port1PortAi.xml, Component1ComponentAi.xml, or Enum1EnumAi.xml in Autocoders/Python/test/enum_xml")
        print ("-------Program Output-------")
        print (p.before)
        assert False

