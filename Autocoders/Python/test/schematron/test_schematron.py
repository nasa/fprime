"""
test_schematron.py:

Checks that the schematron RNG schema files for topology, component, and dictionary xml are properly running / asserting based on various criteria.

@author jishii
"""

import os
import sys
import shutil

sys.path.append(os.environ['BUILD_ROOT'] + os.sep + "Fw" + os.sep + "Python" + os.sep +"src")
sys.path.append(os.environ['BUILD_ROOT'] + os.sep + "Gds" + os.sep + "src") # Add GDS modules
from fprime_gds.common.loaders.xml_loader import XmlLoader

from fprime_ac.parsers import XmlTopologyParser

import subprocess
from subprocess import CalledProcessError
import pexpect
from pexpect import TIMEOUT, EOF

import filecmp
import logging
import time

def test_schematron():
    """
    Tests that schematron files are properly running and asserting
    """
    try:
        
        # cd into test directory to find test files (code/test/schematron can only find files this way)
        curdir = os.getcwd()
        testdir = os.environ['BUILD_ROOT'] + os.sep + "Autocoders" + os.sep
        testdir = testdir + "Python" + os.sep + "test" + os.sep + "schematron" + os.sep + "xml"
        os.chdir(testdir)
        
        bindir = os.environ['BUILD_ROOT'] + os.sep + "Autocoders" + os.sep + "Python" + os.sep + "bin" + os.sep
        
        # Autocode enum XML
        p_enum = pexpect.spawn("python " + bindir + "codegen.py -v Enum1EnumAi.xml")
        p_enum.expect("(?=.*Enum1 Schematron).*")
        print("Enum autocoded for test cases")
        
        
        #####################################################
        ## Running codegen on each of the test topologies
        ## should cause TestTopologyAppAi.xml to successfully
        ## run, Test2TopologyAppAi.xml to fail on async_input
        ## port check, Test3Topology to fail on command opcode
        ## check, and BrokenTopology to fail on window_id check
        
        # Successful test case
        p_test1 = pexpect.spawn("python " + bindir + "codegen.py -v TestTopologyAppAi.xml")
        p_test1.expect("(?=.*Found component XML file)(?=.*Parsing Component TestComponent)(?!.*ERROR)(?!.*is not valid according to schematron).*", timeout=5)
        print("Autocoded TestTopologyAppAi.xml without errors")
        
        # Active component without an async port
        p_test2 = pexpect.spawn("python " + bindir + "codegen.py -v Test2TopologyAppAi.xml")
        p_test2.expect("(?=.*Found component XML file)(?=.*active_comp_schematron.rng)(?!.*ERROR).*", timeout=5)
        print("Autocoded Test2TopologyAppAi.xml and correctly failed on active_comp_schematron.rng check.")
        
        # Topology with 2 instances of the same ID
        p_test3 = pexpect.spawn("python " + bindir + "codegen.py -v BrokenTopologyAppAi.xml")
        p_test3.expect("(?=.*top_uniqueness_schematron.rng)(?!.*ERROR).*", timeout=5)
        print("Autocoded BrokenTopologyAppAi.xml and correctly failed on top_uniqueness_schematron.rng check.")
        
        dictionary_types = ["Cmd", "Evr", "Tlm", "Param"]
        # Broken imported dictionary files / component xml with duplicate ID/op's
        for type in dictionary_types:
            # Broken imported dict
            print("python " + bindir + "codegen.py -v Test{}DictComponentAi.xml".format(type))
            p_test_dict = pexpect.spawn("python " + bindir + "codegen.py -v Test{}DictComponentAi.xml".format(type))
            if type == "Cmd":
                p_test_dict.expect("(?=.*Parsing Component TestComponent)(?=.*command_op_schematron.rng)(?!.*ERROR).*", timeout=5)
            elif type == "Evr":
                p_test_dict.expect("(?=.*Parsing Component TestComponent)(?=.*event_id_schematron.rng)(?!.*ERROR).*", timeout=5)
            elif type == "Tlm":
                p_test_dict.expect("(?=.*Parsing Component TestComponent)(?=.*channel_id_schematron.rng)(?!.*ERROR).*", timeout=5)
            elif type == "Param":
                p_test_dict.expect("(?=.*Parsing Component TestComponent)(?=.*parameter_id_schematron.rng)(?!.*ERROR).*", timeout=5)
            
            print("Autocoded Test{}DictComponentAi.xml and correctly failed on dictionary XML ID/opcode check.".format(type))
        
            # Broken component xml
            p_test_dict = pexpect.spawn("python " + bindir + "codegen.py -v Test{}ComponentAi.xml".format(type))
            p_test_dict.expect("(?=.*comp_uniqueness_schematron.rng)(?!.*ERROR).*", timeout=5)
            print("Autocoded Test{}ComponentAi.xml and correctly failed on component XML ID/opcode check.".format(type))

        shutil.rmtree("DefaultDict")
        os.remove("Enum1EnumAc.cpp")
        os.remove("Enum1EnumAc.hpp")
    
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
