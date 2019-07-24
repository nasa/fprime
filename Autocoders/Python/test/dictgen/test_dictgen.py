"""
test_enum.py:

Checks that the dictgen tools are properly generating python/xml dicts and.

@author jishii
"""

import os
import sys

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

def remove_non_src_files(files):
    src_files = []
    for file in files:
        if file[:4] == "Inst":
            src_files.append(file)

    return src_files

def check_generated_files(testdir):
    # Check that everything from parsed topology was generated
    topology = XmlTopologyParser.XmlTopologyParser("TestTopologyAppAi.xml").get_instances()
    
    inst1 = topology[0]
    inst2 = topology[1]
    inst1_events = [o.get_name() for o in inst1.get_comp_xml().get_events()]
    inst2_events = [o.get_name() for o in inst2.get_comp_xml().get_events()]
    inst1_commands = [o.get_mnemonic() for o in inst1.get_comp_xml().get_commands()]
    inst2_commands = [o.get_mnemonic() for o in inst2.get_comp_xml().get_commands()]
    inst1_channels = [o.get_name() for o in inst1.get_comp_xml().get_channels()]
    inst2_channels = [o.get_name() for o in inst2.get_comp_xml().get_channels()]
    inst1_parameters = [o.get_name() for o in inst1.get_comp_xml().get_parameters()]
    inst2_parameters = [o.get_name() for o in inst2.get_comp_xml().get_parameters()]
    
    dict_parser = XmlLoader()
    parsed_dict = dict_parser.get_xml_tree("TestTopologyAppDictionary.xml")
    parsed_commands = dict_parser.get_xml_section("commands", parsed_dict)
    parsed_events = dict_parser.get_xml_section("events", parsed_dict)
    parsed_commands = dict_parser.get_xml_section("commands", parsed_dict)
    parsed_channels = dict_parser.get_xml_section("channels", parsed_dict)
    parsed_parameters = dict_parser.get_xml_section("parameters", parsed_dict)
    
    # Check that all items are the same from the topology and the generated dict
    for event in parsed_events:
        evr_comp = event.values()[0]
        evr_name = event.values()[1]
        
        if evr_comp == "Inst1":
            if not evr_name in inst1_events:
                print("ERROR: Event in Inst1 not found: {}".format(evr_name))
                assert False
        elif evr_comp == "Inst2":
            if not evr_name in inst2_events:
                print("ERROR: Event in Inst2 not found: {}".format(evr_name))
                assert False
        else:
            print("ERROR: Invalid component name {}".format(evr_comp))
            assert False
            
        if not len(inst1_events) + len(inst2_events) == len(parsed_events):
            print("ERROR: Not all events in topology were found in generated dict")
            assert False
    for command in parsed_commands:
        cmd_comp = command.values()[0]
        cmd_name = command.values()[1]
        
        if cmd_comp == "Inst1":
            if not cmd_name in inst1_commands:
                print("ERROR: Command in Inst1 not found: {}".format(cmd_name))
                assert False
        elif cmd_comp == "Inst2":
            if not cmd_name in inst2_commands:
                print("ERROR: Command in Inst2 not found: {}".format(cmd_name))
                assert False
        else:
            print("ERROR: Invalid component name {}".format(cmd_comp))
            assert False
            
        if not len(inst1_commands) + len(inst2_commands) == len(parsed_commands):
            print("ERROR: Not all commands in topology were found in generated dict")
            assert False
    for channel in parsed_channels:
        chan_comp = channel.values()[0]
        chan_name = channel.values()[1]
        
        if chan_comp == "Inst1":
            if not chan_name in inst1_channels:
                print("ERROR: Channel in Inst1 not found: {}".format(chan_name))
                assert False
        elif chan_comp == "Inst2":
            if not chan_name in inst2_channels:
                print("ERROR: Channel in Inst2 not found: {}".format(chan_name))
                assert False
        else:
            print("ERROR: Invalid component name {}".format(chan_comp))
            assert False

        if not len(inst1_channels) + len(inst2_channels) == len(parsed_channels):
            print("ERROR: Not all channels in topology were found in generated dict")
            assert False
    for parameter in parsed_parameters:
        param_comp = parameter.values()[0]
        param_name = parameter.values()[1]
        
        if param_comp == "Inst1":
            if not param_name in inst1_parameters:
                print("ERROR: Parameter in Inst1 not found: {}".format(param_name))
                assert False
        elif param_comp == "Inst2":
            if not param_name in inst2_parameters:
                print("ERROR: Parameter in Inst2 not found: {}".format(param_name))
                assert False
        else:
            print("ERROR: Invalid component name {}".format(param_comp))
            assert False

        if not len(inst1_parameters) + len(inst2_parameters) == len(parsed_parameters):
            print("ERROR: Not all parameter in topology were found in generated dict")
            assert False

    print("Generated Dictionary consistent with Topology")

    ##################################################
    # Check Pymodule consistency
    
    # Check commands
    expected_cmd_modules = ["Inst1_" + o + ".py" for o in inst1_commands] + ["Inst2_" + o + ".py" for o in inst2_commands]
    if os.path.exists(testdir + os.sep + "commands"):
        files = os.listdir(testdir + os.sep + "commands")
        files = remove_non_src_files(files)
        for mod in expected_cmd_modules:
            if not mod in files:
                print("ERROR: python module {} not found in dictgen/commands".format(mod))
                assert False
            
        if not len(expected_cmd_modules) == len(files):
            print("ERROR: Not all command python modules were generated")
            assert False
    else:
        if not len(expected_cmd_modules) == 0:
            print("ERROR: Command python modules were not generated")
            assert False

    # Check channels
    expected_chan_modules = ["Inst1_" + o + ".py" for o in inst1_channels] + ["Inst2_" + o + ".py" for o in inst2_channels]
    if os.path.exists(testdir + os.sep + "channels"):
        files = os.listdir(testdir + os.sep + "channels")
        files = remove_non_src_files(files)
        for mod in expected_chan_modules:
            if not mod in files:
                print("ERROR: python module {} not found in dictgen/channels".format(mod))
                assert False
        
        if not len(expected_chan_modules) == len(files):
            print("ERROR: Not all channel python modules were generated")
            assert False
    else:
        if not len(expected_chan_modules) == 0:
            print("ERROR: Channel python modules were not generated")
            assert False
        
    # Check events
    expected_evr_modules = ["Inst1_" + o + ".py" for o in inst1_events] + ["Inst2_" + o + ".py" for o in inst2_events]
    if os.path.exists(testdir + os.sep + "events"):
        files = os.listdir(testdir + os.sep + "events")
        files = remove_non_src_files(files)
        for mod in expected_evr_modules:
            if not mod in files:
                print("ERROR: python module {} not found in dictgen/events".format(mod))
                assert False
        
        if not len(expected_evr_modules) == len(files):
            print("ERROR: Not all event python modules were generated")
            assert False
    else:
        if not len(expected_evr_modules) == 0:
            print("ERROR: Event python modules were not generated")
            assert False
        
    # Check parameters
    expected_param_modules = ["Inst1_" + o + ".py" for o in inst1_parameters] + ["Inst2_" + o + ".py" for o in inst2_parameters]
    if os.path.exists(testdir + os.sep + "parameters"):
        files = os.listdir(testdir + os.sep + "parameters")
        files = remove_non_src_files(files)
        for mod in expected_param_modules:
            if not mod in files:
                print("ERROR: python module {} not found in dictgen/parameters".format(mod))
                assert False
        
        if not len(expected_param_modules) == len(files):
            print("ERROR: Not all parameter python modules were generated")
            assert False
    else:
        if not len(expected_param_modules) == 0:
            print("ERROR: Parameter python modules were not generated")
            assert False

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
        
        check_generated_files(testdir)

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
