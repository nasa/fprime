"""
test_dictgen.py:

Checks that the dictgen tools are properly generating python/xml dicts.

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
    remove_headers(filename)
    
    if not (filecmp.cmp(filename,"templates/{}".format(filename + ".txt"))):
        print("WARNING: {} generated incorrectly according to Autocoders/Python/test/dictgen/templates/{}".format(filename, filename + ".txt"))
        diff_lines = file_diff(filename, "templates/{}".format(filename) + ".txt")
        print("WARNING: the following lines from " + filename + " differ from the template: " + str(diff_lines))
    else:
        print("{} is consistent with expected template".format(filename))

def filter_non_src_files(files):
    """
    Filters out all files which don't contain prefix Inst
    i.e. all files that aren't pymod dicts for TestComponent
    """
    src_files = []
    for file in files:
        if file[:4] == "Inst":
            src_files.append(file)

    return src_files

def get_serializables_from_comp_xml(comp_xml):
    """
    Returns list containing all serialized types found from
    component xml and its imported files... NOTE: Must add
    check for serial types found outside imported serial files
    """
    serial_imports = comp_xml.get_serializable_type_files()
    
    # Imports = lxml serials from imported serializable types
    serial_list = {
        "imports": []
    }

    loader = XmlLoader()
    # Find serializables in imported file
    for serial_file in serial_imports:
        serial_path = os.environ['BUILD_ROOT'] + os.sep + serial_file
        if os.path.exists(serial_path):
            opened_serial = loader.get_xml_tree(serial_path)
            find_serial_from_serial(opened_serial, serial_list, "imports")
        else:
            print("ERROR: invalid comp_xml with serial import {}".format(serial_path))
            assert False

    return serial_list

def get_enums_from_comp_xml(comp_xml):
    """
    Returns list containing all enums found from component xml
    and its imported files... NOTE: This list will not be complete
    as it does not contain any enums defined in PortAi.xml files.
    For the scope of this test, only arguments and dictionaries are
    checked and ports are ignored
    """
    dict_imports = comp_xml.get_imported_dictionary_files()
    
    # Imports = lxml enums from imported dicts
    # Compxml = FPrime enums from XMLComponentParser
    enum_list = {
        "imports": [],
        "compxml": []
    }

    loader = XmlLoader()
    # Find enums in imported dictionary
    for dict_file in dict_imports:
        dict_path = os.environ['BUILD_ROOT'] + os.sep + dict_file
        if os.path.exists(dict_path):
            opened_dict = loader.get_xml_tree(dict_path)
            find_enum_from_dict(opened_dict, enum_list, "imports")
        else:
            print("ERROR: invalid comp xml dict import {}".format(dict_path))
            assert False

    # Find enum within component xml
    comp_cmds = comp_xml.get_commands()
    comp_evrs = comp_xml.get_events()
    comp_chans = comp_xml.get_channels()
    comp_params = comp_xml.get_parameters()

    for cmd in comp_cmds:
        for arg in cmd.get_args():
            if type(arg.get_type()) is tuple:
                enum_list["compxml"].append(arg.get_type())

    for evr in comp_evrs:
        for arg in evr.get_args():
            if type(arg.get_type()) is tuple:
                enum_list["compxml"].append(arg.get_type())

    for chan in comp_chans:
        if type(chan.get_type()) is tuple:
            enum_list["compxml"].append(chan.get_type())

    for param in comp_params:
        if type(param.get_type()) is tuple:
            enum_list["compxml"].append(param.get_type())

    return enum_list

def find_serial_from_serial(opened_serial, serial_list, type):
    """
    Takes in an lxml tree for a dictionary and adds every
    serializable within the tree to the argument "serial_list"
    """
    if opened_serial.tag == "enums":
        for serial in opened_serial.getchildren():
            serial_list[type].append(serial)

def find_enum_from_dict(opened_dict, enum_list, type):
    """
    Takes in an lxml tree for a dictionary and adds every
    enum within the tree to the argument "enum_list"
    """
    if opened_dict.tag == "commands":
        for channel in opened_dict.getchildren():
            for child in channel.getchildren():
                if child.tag == "args":
                    for arg in child.getchildren():
                        if "type" in arg.attrib:
                            if arg.attrib["type"] == "ENUM":
                                enum_list[type].append(arg.getchildren()[0])
    elif opened_dict.tag == "events":
        for event in opened_dict.getchildren():
            for child in event.getchildren():
                if child.tag == "args":
                    for arg in child.getchildren():
                        if "type" in arg.attrib:
                            if arg.attrib["type"] == "ENUM":
                                enum_list[type].append(arg.getchildren()[0])
    elif opened_dict.tag == "telemetry":
        for channel in opened_dict.getchildren():
            if "data_type" in channel.attrib:
                if channel.attrib["data_type"] == "ENUM":
                    enum_list[type].append(channel.getchildren()[0])
    elif opened_dict.tag == "parameters":
        for param in opened_dict.getchildren():
            if "data_type" in param.attrib:
                if param.attrib["data_type"] == "ENUM":
                    enum_list[type].append(param.get_children()[0])

def check_generated_files(testdir):
    """
    Compares generated dictionary and pymods with original topology xml
    """
    # Check that everything from parsed topology was generated
    topology = XmlTopologyParser.XmlTopologyParser("TestTopologyAppAi.xml").get_instances()
    
    # Original TopologyAi.xml file
    inst1 = topology[0]
    inst2 = topology[1]
    
    # Dict objects
    inst1_events = [o for o in inst1.get_comp_xml().get_events()]
    inst2_events = [o for o in inst2.get_comp_xml().get_events()]
    inst1_commands = [o for o in inst1.get_comp_xml().get_commands()]
    inst2_commands = [o for o in inst2.get_comp_xml().get_commands()]
    inst1_channels = [o for o in inst1.get_comp_xml().get_channels()]
    inst2_channels = [o for o in inst2.get_comp_xml().get_channels()]
    inst1_parameters = [o for o in inst1.get_comp_xml().get_parameters()]
    inst2_parameters = [o for o in inst2.get_comp_xml().get_parameters()]

    # Names of dict objects
    inst1_event_names = [o.get_name() for o in inst1_events]
    inst2_event_names = [o.get_name() for o in inst2_events]
    inst1_command_names = [o.get_mnemonic() for o in inst1_commands]
    inst2_command_names = [o.get_mnemonic() for o in inst2_commands]
    inst1_channel_names = [o.get_name() for o in inst1_channels]
    inst2_channel_names = [o.get_name() for o in inst2_channels]
    inst1_parameter_names = [o.get_name() for o in inst1_parameters]
    inst2_parameter_names = [o.get_name() for o in inst2_parameters]
    
    # Parse out all arguments for commands and events
    inst1_command_args = []
    for cmd in inst1_commands:
        for arg in cmd.get_args():
            inst1_command_args.append(arg)
    inst2_command_args = []
    for cmd in inst1_commands:
        for arg in cmd.get_args():
            inst2_command_args.append(arg)
    inst1_event_args = []
    for evr in inst1_events:
        for arg in evr.get_args():
            inst1_event_args.append(arg)
    inst2_event_args = []
    for evr in inst2_events:
        for arg in evr.get_args():
            inst2_event_args.append(arg)
    
    # Command / Event names
    inst1_command_arg_names = [arg.get_name() for arg in inst1_command_args]
    inst2_command_arg_names = [arg.get_name() for arg in inst2_command_args]
    inst1_event_arg_names = [arg.get_name() for arg in inst1_event_args]
    inst2_event_arg_names = [arg.get_name() for arg in inst2_event_args]

    # Dict object types
    inst1_channel_types = [channel.get_type() for channel in inst1_channels]
    inst2_channel_types = [channel.get_type() for channel in inst2_channels]
    inst1_command_arg_types = [arg.get_type() for arg in inst1_command_args]
    inst2_command_arg_types = [arg.get_type() for arg in inst2_command_args]
    inst1_event_arg_types = [arg.get_type() for arg in inst1_event_args]
    inst2_event_arg_types = [arg.get_type() for arg in inst2_event_args]
    inst1_parameter_types = [param.get_type() for param in inst1_parameters]
    inst2_parameter_types = [param.get_type() for param in inst2_parameters]
    
    # Enums
    inst1_enums = get_enums_from_comp_xml(inst1.get_comp_xml())
    inst2_enums = get_enums_from_comp_xml(inst2.get_comp_xml())
    inst_enums = {}
    inst_enums["compxml"] = inst1_enums["compxml"] + inst2_enums["compxml"]
    inst_enums["imports"] = inst1_enums["imports"] + inst2_enums["imports"]

    # Serializables
    inst1_serials = get_serializables_from_comp_xml(inst1.get_comp_xml())
    inst2_serials = get_serializables_from_comp_xml(inst2.get_comp_xml())
    inst_serials = {}
    inst_serials["imports"] = inst1_serials["imports"] + inst2_serials["imports"]
    
    # GDS XML Dictionary
    dict_parser = XmlLoader()
    parsed_dict = dict_parser.get_xml_tree("TestTopologyAppDictionary.xml")
    parsed_commands = dict_parser.get_xml_section("commands", parsed_dict)
    parsed_events = dict_parser.get_xml_section("events", parsed_dict)
    parsed_commands = dict_parser.get_xml_section("commands", parsed_dict)
    parsed_channels = dict_parser.get_xml_section("channels", parsed_dict)
    parsed_parameters = dict_parser.get_xml_section("parameters", parsed_dict)
    parsed_serializables = dict_parser.get_xml_section("serializables", parsed_dict)
    parsed_enums = dict_parser.get_xml_section("enums", parsed_dict)

    ###################################################################
    # Run Enum Check

    # Compare TopAi.xml enums with GDS XML Dict enums
    compare_enums_ai_gds(inst_enums, parsed_enums)

    ###################################################################
    # Run Serializable Check

    # Compare TopAi.xml serializables with GDS XML Dict serializables
    compare_serials_ai_gds(inst_serials, parsed_serializables)

    ###################################################################
    # Before deep comparison between elements, check that number
    # of elements is same between dict and topology

    # Check if same # evrs, tlm, cmd, param
    if not len(inst1_events) + len(inst2_events) == len(parsed_events):
        print("ERROR: Not all events in topology were found in generated dict")
        assert False
    if not len(inst1_commands) + len(inst2_commands) == len(parsed_commands):
        print("ERROR: Not all commands in topology were found in generated dict")
        assert False
    if not len(inst1_channels) + len(inst2_channels) == len(parsed_channels):
        print("ERROR: Not all channels in topology were found in generated dict")
        assert False
    if not len(inst1_parameters) + len(inst2_parameters) == len(parsed_parameters):
        print("ERROR: Not all parameters in topology were found in generated dict")
        assert False
    
    ###################################################################
    # Check that all items are the same from the topology and the generated dict
    
    # Check events
    for event in parsed_events:
        evr_comp = event.attrib["component"]
        evr_name = event.attrib["name"]
        
        if evr_comp == "Inst1":
            if not evr_name in inst1_event_names:
                print("ERROR: Event in Inst1 not found: {}".format(evr_name))
                assert False
        elif evr_comp == "Inst2":
            if not evr_name in inst2_event_names:
                print("ERROR: Event in Inst2 not found: {}".format(evr_name))
                assert False
        else:
            print("ERROR: Invalid component name {}".format(evr_comp))
            assert False
                
        # Arg check
        for args in event:
            if len(args) > 0:
                for arg in args:
                    arg_name = arg.attrib["name"]
                    arg_type = arg.attrib["type"]
                    if not (arg_name in inst1_event_arg_names or arg_name in inst2_event_arg_names):
                        print("ERROR: event arg name {} for arg {} not found ".format(arg_name, evr_name) +
                                  "in topologyAi.xml")
                        assert False
                    if not (arg_type in inst1_event_arg_types or arg_type in inst2_event_arg_types):
                        print("ERROR: event arg type {} for arg {} not found ".format(arg_type, evr_name))
                        assert False
       
    # Check commands
    for command in parsed_commands:
        cmd_comp = command.attrib["component"]
        cmd_name = command.attrib["mnemonic"]
        
        if cmd_comp == "Inst1":
            if not cmd_name in inst1_command_names:
                print("ERROR: Command in Inst1 not found: {}".format(cmd_name))
                assert False
        elif cmd_comp == "Inst2":
            if not cmd_name in inst2_command_names:
                print("ERROR: Command in Inst2 not found: {}".format(cmd_name))
                assert False
        else:
            print("ERROR: Invalid component name {}".format(cmd_comp))
            assert False

        # Arg check
        for args in command:
            if len(args) > 1:
                for arg in args:
                    arg_name = arg.attrib["name"]
                    arg_type = arg.attrib["type"]
                    if not (arg_name in inst1_command_arg_names or arg_name in inst2_command_arg_names):
                        print("ERROR: event arg name {} for arg {} not found ".format(arg_name, cmd_name) +
                              "in topologyAi.xml")
                        assert False
                            
                    if not (arg_type in inst1_command_arg_types or arg_type in inst2_command_arg_types):
                        print("ERROR: event arg type {} for arg {} not found ".format(arg_type, evr_name) +
                              "in topologyAi.xml")
                        assert False

    # Check channels
    for channel in parsed_channels:
        chan_comp = channel.attrib["component"]
        chan_name = channel.attrib["name"]
        
        if chan_comp == "Inst1":
            if not chan_name in inst1_channel_names:
                print("ERROR: Channel in Inst1 not found: {}".format(chan_name))
                assert False
        elif chan_comp == "Inst2":
            if not chan_name in inst2_channel_names:
                print("ERROR: Channel in Inst2 not found: {}".format(chan_name))
                assert False
        else:
            print("ERROR: Invalid component name {}".format(chan_comp))
            assert False
        # Type check
        chan_type = channel.attrib["type"]
        if not (chan_type in inst1_channel_types or chan_type in inst2_channel_types):
            print("ERROR: Channel type {} in channel {} not found in topologyAi.xml".format(chan_type, chan_name))
            assert False
                
    # Check parameters
    for parameter in parsed_parameters:
        param_comp = parameter.attrib["component"]
        param_name = parameter.attrib["name"]
        
        if param_comp == "Inst1":
            if not param_name in inst1_parameter_names:
                print("ERROR: Parameter in Inst1 not found: {}".format(param_name))
                assert False
        elif param_comp == "Inst2":
            if not param_name in inst2_parameter_names:
                print("ERROR: Parameter in Inst2 not found: {}".format(param_name))
                assert False
        else:
            print("ERROR: Invalid component name {}".format(param_comp))
            assert False
        # Type check
        param_type = parameter.attrib["type"]
        if not (param_type in inst1_parameter_types or param_type in inst2_parameter_types):
            print("ERROR: Parameter type {} in param {} not found in topologyAi.xml".format(param_type, param_name))
            assert False

    print("Checked dictionary item names, types and arguments")

    print("Generated Dictionary consistent with Topology")

    ##################################################
    # Check Pymodule consistency
    
    # Check commands
    expected_cmd_modules = ["Inst1_" + o + ".py" for o in inst1_command_names] + ["Inst2_" + o + ".py" for o in inst2_command_names]
    if os.path.exists(testdir + os.sep + "commands"):
        files = os.listdir(testdir + os.sep + "commands")
        files = filter_non_src_files(files)
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
    expected_chan_modules = ["Inst1_" + o + ".py" for o in inst1_channel_names] + ["Inst2_" + o + ".py" for o in inst2_channel_names]
    if os.path.exists(testdir + os.sep + "channels"):
        files = os.listdir(testdir + os.sep + "channels")
        files = filter_non_src_files(files)
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
    expected_evr_modules = ["Inst1_" + o + ".py" for o in inst1_event_names] + ["Inst2_" + o + ".py" for o in inst2_event_names]
    if os.path.exists(testdir + os.sep + "events"):
        files = os.listdir(testdir + os.sep + "events")
        files = filter_non_src_files(files)
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
    expected_param_modules = ["Inst1_" + o + ".py" for o in inst1_parameter_names] + ["Inst2_" + o + ".py" for o in inst2_parameter_names]
    if os.path.exists(testdir + os.sep + "parameters"):
        files = os.listdir(testdir + os.sep + "parameters")
        files = filter_non_src_files(files)
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

def compare_serials_ai_gds(topology_serials, gds_serials):
    """
    Checks that serials foundin gds dict are all in topology
    topology_serials is list of type FPrime serializable,
    gds_serials is list of type lxml serializable node
    """
    types = []
    member_types = {}
    member_names = {}
    for gds_serial in gds_serials:
        # For scope of test, only verify serial type and member names/types
        type = gds_serial.attrib["type"].split("::")
        type = type[len(type) - 1]
        types.append(type)
        if not type in member_names.keys():
            member_names[type] = []
        if not type in member_types.keys():
            member_types[type] = []
        for members in gds_serial:
            for member in members:
                print(member.tag)
                member_names[type].append(member.attrib["name"])
                member_types[type].append(member.attrib["type"])

    for top_serial in topology_serials["imports"]:
        # Check that top_serial's type attribute is found in gds serial types list
        top_serial_type = top_serial.attrib["type"]
        if not top_serial_type in types:
            print("ERROR: Serial type {} from topology ai not found in gds dict".format(top_serial_type))
            assert False
        # Check members
        for child in top_serial:
            if "type" in child.keys():
                if not child.attrib["type"] in member_types[top_serial_type]:
                    print("ERROR: Serial member type {} from ".format(child.attrib["type"]) +
                          "topology ai imported dict not found in gds dict")
                    assert False
            if "name" in child.keys():
                if not child.attrib["name"] in member_types[top_serial_type]:
                    print("ERROR: Serial member name {} from ".format(child.attrib["name"]) +
                          "topology ai imported dict not found in gds dict")
    
    print("Serializable check run successfully")

def compare_enums_ai_gds(topology_enums, gds_enums):
    """
    Checks that enums found in gds dict are all in topology
    topology_enums is list of type FPrime enum, gds_enums is
    list of type lxml enum node
    """
    names = []
    items = {}
    for gds_enum in gds_enums:
        # For scope of test, only verify enum name and item names
        name = gds_enum.attrib["type"].split("::")
        name = name[len(name) - 1]
        names.append(name)
        if not name in items.keys():
            items[name] = []
        for item in gds_enum:
            items[name].append(item.attrib["name"])

    for top_enum in topology_enums["compxml"]:
        # Check that top_enum's name attribute is found in gds enum names list
        top_enum_name = top_enum[0][1]
        if not top_enum_name in names:
            print("ERROR: Enum name {} from topology ai not found in gds dict".format(top_enum_name))
            assert False
        # Check item names
        for item in top_enum[1]:
            if not item[0] in items[top_enum_name]:
                print("ERROR: Enum {} item name {} not found in gds dict".format(top_enum_name, item[0]))
                assert False

    for top_enum in topology_enums["imports"]:
        # Check that top_enum's name attribute is found in gds enum names list
        top_enum_name = top_enum.attrib["name"]
        if not top_enum_name in names:
            print("ERROR: Enum name {} from topology ".format(top_enum_name) +
                  "ai imported dict not found in gds dict")
            assert False
        # Check item names
        for child in top_enum:
            if "name" in child.keys():
                if not child.attrib["name"] in items[top_enum_name]:
                    print("ERROR: Enum item name {} from ".format(child.attrib["name"]) +
                          "topology ai imported dict not found in gds dict")
                    assert False

    print("Enum check run successfully")

def test_dictgen():
    """
    Tests that tests are being generated correctly
    """
    try:
        
        # cd into test directory to find test files (code/test/dictgen can only find files this way)
        curdir = os.getcwd()
        testdir = os.environ['BUILD_ROOT'] + os.sep + "Autocoders" + os.sep
        testdir = testdir + "Python" + os.sep + "test" + os.sep + "dictgen"
        os.chdir(testdir)
        
        bindir = os.environ['BUILD_ROOT'] + os.sep + "Autocoders" + os.sep + "Python" + os.sep + "bin" + os.sep
        
        ## Spawn executable
        p_pymod = pexpect.spawn("python " + bindir + "pymod_dictgen.py -v TestTopologyAppAi.xml")

        p_pymod.expect("(?=.*Generated component dicts for DictGen::TestComponent)(?=.*Generated pymods for topology TestTopology)(?!.*ERROR).*", timeout=5)
        
        print("Autocoded TestTopology using pymod_dictgen dictgen tool")
        
        p_gds = pexpect.spawn("python " + bindir + "gds_dictgen.py -v TestTopologyAppAi.xml")
        
        p_gds.expect("(?=.*Generated XML dictionary TestTopologyAppDictionary.xml)(?!.*ERROR).*", timeout=5)
        
        print("Autocoded TestTopology using gds_dictgen dictgen tool")
        
        time.sleep(3)
        
        gen_pymods = []
        gen_pymods.append("channels/Inst1_Test_Tlm0.py")
        gen_pymods.append("channels/Inst2_Test_Tlm0.py")
        gen_pymods.append("commands/Inst1_Test_Cmd0.py")
        gen_pymods.append("commands/Inst1_Test_Cmd1.py")
        gen_pymods.append("commands/Inst1_Test_Cmd2.py")
        gen_pymods.append("commands/Inst1_Test_Cmd3.py")
        gen_pymods.append("commands/Inst2_Test_Cmd0.py")
        gen_pymods.append("commands/Inst2_Test_Cmd1.py")
        gen_pymods.append("commands/Inst2_Test_Cmd2.py")
        gen_pymods.append("commands/Inst2_Test_Cmd3.py")
        gen_pymods.append("events/Inst1_Test_Evr0.py")
        gen_pymods.append("events/Inst1_Test_Evr1.py")
        gen_pymods.append("events/Inst1_Test_Evr2.py")
        gen_pymods.append("events/Inst2_Test_Evr0.py")
        gen_pymods.append("events/Inst2_Test_Evr1.py")
        gen_pymods.append("events/Inst2_Test_Evr2.py")
        
        # Test whether all generated files match expected
        compare_genfile("TestTopologyAppDictionary.xml")
        for pymod in gen_pymods:
            compare_genfile(pymod)
        
        # Evaluate imports on each of the pymods to show they work properly
        dictgen_dir = os.environ['BUILD_ROOT'] + os.sep + "Autocoders" + os.sep + "Python"
        dictgen_dir += os.sep + "test" + os.sep + "dictgen" + os.sep
        sys.path.append(dictgen_dir)
        
        # TODO: FIGURE OUT HOW TO MAKE SERIALIZED TYPE IMPORTS WORK
#        for pymod in gen_pymods:
#            exec("import {}".format(pymod.replace("/",".").replace(".py","")))
#        print("Executed all pymod imports successfully")

        check_generated_files(testdir)
        
        # Remove all generated files
        if os.path.exists("TestTopologyAppDictionary.xml"):
            os.remove("TestTopologyAppDictionary.xml")
        if os.path.exists("TestTopologyAppAi_IDTableLog.txt"):
            os.remove("TestTopologyAppAi_IDTableLog.txt")
        if os.path.exists("channels"):
            shutil.rmtree("channels")
        if os.path.exists("commands"):
            shutil.rmtree("commands")
        if os.path.exists("events"):
            shutil.rmtree("events")

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
