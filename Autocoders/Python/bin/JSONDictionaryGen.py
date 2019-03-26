'''
NAME: JSONDictonaryGen.py

DESCRIPTION: Reads topology XML to produce command, EVR, and channel JSON
             dictionaries

AUTHOR: Aaron Doubek-Kraft aarondou@jpl.nasa.gov
'''

import os
import logging
import json
import sys

from optparse import OptionParser

from fprime_ac.models import ModelParser
from fprime_ac.parsers import XmlTopologyParser
from fprime_ac.parsers import XmlSerializeParser
from fprime_ac.utils import Logger

# Version label for now
class Version:
    id      = "0.1"
    comment = "Initial prototype"
VERSION = Version()

def format_type_item(typeItem):
    typeIsTuple = (str(type(typeItem)) == '<type \'tuple\'>')
    if typeIsTuple:
        description = typeItem[0]
        values = typeItem[1]
        valuesArr = []
        for value in values:
            valuesArr.append({
                "name": value[0],
                "value": value[1],
                "comment": value[2]
            })

        typeObj = {
            "type": description[0],
            "name": description[1],
            "values": valuesArr,
        }
    else:
        typeObj = typeItem

    return typeObj

def pinit():
    """
    Initialize the option parser and return it.
    """

    current_dir = os.getcwd()

    usage = "usage: %prog [options] [xml_filename]"
    vers = "%prog " + VERSION.id + " " + VERSION.comment
    program_longdesc = '''
        This script reads F' topology XML and produces dictionaries represented as
        JSON. These documents contain all command, evr, and channel telemetry
        descriptions.
        '''
    program_license = "Copyright 2018 aarondou (California Institute of Technology)                                            \
                ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged."

    parser = OptionParser(usage, version=vers, epilog=program_longdesc,description=program_license)

    parser.add_option("-p", "--path", dest="work_path", type="string",
        help="Switch to new working directory (def: %s)." % current_dir,
        action="store", default=current_dir)

    parser.add_option("-L", "--logger-output-file", dest="logger_output",
        default=None, help="Set the logger output file. (def: stdout).")

    return parser

def main():

    parser = pinit()
    (opts, args) = parser.parse_args()

    Logger.connectOutputLogger(opts.logger_output)

    # Global logger init. below.
    PRINT = logging.getLogger('output')
    DEBUG = logging.getLogger('debug')

    #
    #  Parse the input Component XML file and create internal meta-model
    #
    if len(args) == 0:
        PRINT.info("\nUsage: %s [options] xml_filename" % sys.argv[0])
        PRINT.info("ERROR: Cannot create dictionary\n")
        sys.exit(-1)
        return
    else:
        xmlFilename = args[0]

    # Check for BUILD_ROOT env. variable
    if ('BUILD_ROOT' in list(os.environ.keys())) == False:
        PRINT.info("ERROR: The -b command option requires that BUILD_ROOT environmental variable be set to root build path...")
        sys.exit(-1)
    else:
        BUILD_ROOT = os.environ['BUILD_ROOT']
        ModelParser.BUILD_ROOT = BUILD_ROOT
        PRINT.info("BUILD_ROOT set to %s in environment" % BUILD_ROOT)

    parsedTopology = XmlTopologyParser.XmlTopologyParser(xmlFilename)
    deployment = parsedTopology.get_deployment()
    outFilename = deployment + "Dictionary.json"
    outFilepath = "/".join([opts.work_path, outFilename])
    descriptionFilename = "/".join([opts.work_path, "/dictPath.txt"])

    dictionary = {}
    dictionary[deployment] = {
        "events": {},
        "channels": {},
        "commands": {},
        "serializables": {}
    }

    events = dictionary[deployment]["events"]
    channels = dictionary[deployment]["channels"]
    commands = dictionary[deployment]["commands"]
    serializables = dictionary[deployment]["serializables"]
    limitLabels = ["low_red", "low_orange", "low_yellow", "high_yellow", "high_orange", "high_red"]
    unitLabels = ["label", "gain", "offset"]

    instanceIDs = {
        "events": {},
        "channels": {},
        "commands": {}
    }

    for inst in parsedTopology.get_instances():
        serializableFilenames = inst.get_comp_xml().get_serializable_type_files()
        for filename in serializableFilenames:
            parsedSerializable = XmlSerializeParser.XmlSerializeParser(BUILD_ROOT + '/' + filename)
            name = parsedSerializable.get_name()
            namespace = parsedSerializable.get_namespace()
            members = []
            membersRaw = parsedSerializable.get_members()
            for member in membersRaw:
                members.append({
                    "name": member[0],
                    "type": format_type_item(member[1]),
                    "size": member[2],
                    "format_string": member[3],
                    "comment": member[4]
                })

            metadata = {
                "name": name,
                "namespace": namespace,
                "members": members,
            }

            serializables[namespace + "::" + name] = metadata
        comp_name = inst.get_name()
        comp_type = inst.get_type()
        comp_namespace = inst.get_namespace()
        component = "::".join([comp_namespace, comp_type])
        base_id = inst.get_base_id()
        if '0x' in base_id:
            base_id = int(base_id, 16)
        else:
            base_id = int(base_id)
        comp_parser = inst.get_comp_xml()
        comp_dir = dir(comp_parser)

        if "get_commands" in comp_dir:
            for command in comp_parser.get_commands():
                opcode = command.get_opcodes()[0]
                opcode = int(opcode, 16) if ('0x' in opcode) else int(opcode)
                opcode += base_id

                name = command.get_mnemonic()
                if name in instanceIDs["commands"]:
                    instanceIDs["commands"][name].append(opcode)
                else:
                    instanceIDs["commands"][name] = [opcode]

                arguments = []
                for arg in command.get_args():
                    typeItem = arg.get_type()
                    typeObj = format_type_item(typeItem)
                    arguments.append(typeObj)

                metadata = {
                    "id": opcode,
                    "name": name,
                    "instance": comp_name,
                    "description": command.get_comment(),
                    "component": component,
                    "arguments" : arguments
                }

                commands[opcode] = metadata

        if "get_events" in comp_dir:
            for event in comp_parser.get_events():
                ev_id = event.get_ids()[0]
                ev_id = int(ev_id, 16) if ('0x' in ev_id) else int(ev_id)
                ev_id += base_id

                name = event.get_name()
                if name in instanceIDs["events"]:
                    instanceIDs["events"][name].append(ev_id)
                else:
                    instanceIDs["events"][name] = [ev_id]

                arguments = []
                for arg in event.get_args():
                    typeItem = arg.get_type()
                    typeObj = format_type_item(typeItem)
                    arguments.append(typeObj)

                metadata = {
                    "id": ev_id,
                    "description":  event.get_comment(),
                    "name": name,
                    "instance": comp_name,
                    "component": component,
                    "format_string": event.get_format_string(),
                    "severity": event.get_severity(),
                    "telem_type": "event",
                    "arguments": arguments
                }

                events[ev_id] = metadata

        if "get_channels" in comp_dir:
            for channel in comp_parser.get_channels():
                ch_id = channel.get_ids()[0]
                ch_id = int(ch_id, 16) if ('0x' in ch_id) else int(ch_id)
                ch_id += base_id

                name = channel.get_name()
                if name in instanceIDs["channels"]:
                    instanceIDs["channels"][name].append(ch_id)
                else:
                    instanceIDs["channels"][name] = [ch_id]

                units = []
                for unit in channel.get_units():
                    units.append(dict(list(zip(unitLabels, unit))))

                typeObj = channel.get_type()
                type_name = ''
                if isinstance(typeObj, str):
                    type_name = typeObj
                else:
                    type_name = 'Enum'
                    enum_dict = {}
                    for (i, enum) in enumerate(typeObj[1]):
                        enum_dict[str(i)] = enum[0]


                metadata = {
                    "id": ch_id,
                    "name": name,
                    "instance": comp_name,
                    "description": channel.get_comment(),
                    "telem_type": "channel",
                    "component": component,
                    "format_string": channel.get_format_string(),
                    "limits" : dict(list(zip(limitLabels, channel.get_limits()))),
                    "type": type_name,
                    "units": units
                }

                if (type_name == "Enum"):
                    metadata["enum_dict"] = enum_dict
                    metadata["format_string"] = "%s"

                channels[ch_id] = metadata

    #Prepend instance name to commands, events, and channels with duplicate component types
    # PRINT.info(json.dumps(instanceIDs, indent=4))
    for telemetryType, idDict in list(instanceIDs.items()):
        for name, ids in list(idDict.items()):
            if len(ids) > 1:
                for id in ids:
                    telem = dictionary[deployment][telemetryType][id]
                    name = telem["name"]
                    instanceName = telem["instance"]
                    name = "_".join([instanceName, name])
                    telem["name"] = name

    # Stringify JSON -- indent option makes it readable, can be removed if file
    # size is an issue
    jsonStr = json.dumps(dictionary, indent=4)

    # Create output directory if it doesn't exist
    directory = os.path.dirname(outFilepath)
    if not os.path.exists(directory):
        os.makedirs(directory)

    # Write JSON to file
    outFile = open(outFilepath, 'w')
    outFile.write(jsonStr)
    descriptionFile = open(descriptionFilename, 'w')
    descriptionFile.write(outFilepath)
    PRINT.info("\nJSON output written to %s" % outFilepath)
    outFile.close()

if __name__ == '__main__':
    main()
