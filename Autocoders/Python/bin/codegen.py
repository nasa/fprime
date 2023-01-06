#!/usr/bin/env python3
# ===============================================================================
# NAME: codegen.py
#
# DESCRIPTION: This script is used to generate components, ports and connectors
# from XML definition files.
#
# ===============================================================================
import glob
import logging
import os
import sys
import time
import traceback
from optparse import OptionParser

# Meta-model for Component only generation
from fprime_ac.models import CompFactory, PortFactory, Serialize, TopoFactory

# Parsers to read the XML
from fprime_ac.parsers import (
    XmlComponentParser,
    XmlParser,
    XmlPortsParser,
    XmlSerializeParser,
    XmlTopologyParser,
)
from fprime_ac.utils import (
    ArrayGenerator,
    ConfigManager,
    EnumGenerator,
    Logger,
    TopDictGenerator,
)
from fprime_ac.utils.buildroot import get_build_roots, search_for_file, set_build_roots
from fprime_ac.utils.version import get_fprime_version, get_project_version
from lxml import etree

# Generators to produce the code
try:
    from fprime_ac.generators import GenFactory
except ImportError as ime:
    print("[ERROR] Cheetah templates need to be generated.\n\t", ime, file=sys.stderr)
    sys.exit(1)

# Flag to indicate verbose mode.
VERBOSE = False

# Global logger init. below.
PRINT = logging.getLogger("output")
DEBUG = logging.getLogger("debug")

# Used by unit test to disable things.
TEST = False

# After catching exception this is set True
# so a clean up routine deletes *_ac_*.[ch]
# and *_ac_*.xml files within module.
ERROR = False

# Configuration manager object.
CONFIG = ConfigManager.ConfigManager.getInstance()

# Build a default log file name
SYS_TIME = time.gmtime()

# Deployment name from topology XML only
DEPLOYMENT = None

# Version label for now
class Version:
    id = "0.1"
    comment = "Initial prototype"


VERSION = Version()


def moveCAndHFiles(path_prefix):
    """
    Moves the generated c and h files
    to a specific directory.
    """
    for f in glob.glob("*.c"):
        os.rename(f, os.path.join(path_prefix, f))
    for f in glob.glob("*.h"):
        os.rename(f, os.path.join(path_prefix, f))


def cleanUp():
    """
    If something failed then clean up files generated.
    """
    PRINT.info("ERROR: Cleaning up partially created files.")
    for file in glob.glob("*_ac_*.new"):
        os.remove(file)

    for file in glob.glob("*_token.data"):
        os.remove(file)

    for file in glob.glob("*_opcode_offset.data"):
        os.remove(file)

    PRINT.info("Completed.")

    sys.exit(-1)


def pinit():
    """
    Initialize the option parser and return it.
    """

    current_dir = os.getcwd()

    usage = "usage: %prog [options] [xml_filename]"
    vers = f"%prog {VERSION.id} {VERSION.comment}"

    parser = OptionParser(usage, version=vers)

    parser.add_option(
        "-b",
        "--build_root",
        dest="build_root_flag",
        help="Enable search for environment variable BUILD_ROOT to establish absolute XML directory path",
        action="store_true",
        default=False,
    )

    parser.add_option(
        "-p",
        "--path",
        dest="work_path",
        type="string",
        help=f"Switch to new working directory (def: {current_dir}).",
        action="store",
        default=current_dir,
    )

    parser.add_option(
        "-v",
        "--verbose",
        dest="verbose_flag",
        help="Enable verbose mode showing more runtime detail (def: False)",
        action="store_true",
        default=False,
    )

    parser.add_option(
        "-t",
        "--template",
        dest="impl_flag",
        help="Enable generation of *Impl_[hpp,cpp].template implementation template files (def: False)",
        action="store_true",
        default=False,
    )

    parser.add_option(
        "-u",
        "--unit-test",
        dest="unit_test",
        help="Enable generation of unit test component files (def: False)",
        action="store_true",
        default=False,
    )

    parser.add_option(
        "-l",
        "--logger",
        dest="logger",
        default="QUIET",
        help="Set the logging level <DEBUG | INFO | QUIET> (def: 'QUIET').",
    )

    parser.add_option(
        "-L",
        "--logger-output-file",
        dest="logger_output",
        default=None,
        help="Set the logger output file. (def: defaultgen.log).",
    )

    parser.add_option(
        "-d",
        "--dependency-file",
        dest="dependency_file",
        default=None,
        help="Set the output file for build dependencies",
    )

    parser.add_option(
        "-g",
        "--default_dict",
        dest="default_dict",
        help="Generate default GDS dictionary classes",
        action="store_true",
        default=False,
    )

    parser.add_option(
        "-x",
        "--xml_topology_dict",
        dest="xml_topology_dict",
        help="Generate XML GDS dictionary file",
        action="store_true",
        default=False,
    )

    parser.add_option(
        "-T",
        "--default_topology_dict",
        dest="default_topology_dict",
        help="Generate default GDS topology dictionary classes",
        action="store_true",
        default=False,
    )

    parser.add_option(
        "-o",
        "--dict_dir",
        dest="dict_dir",
        help="Output directory for dictionary. Needed for -g.",
        default=None,
    )

    parser.add_option(
        "-H",
        "--html_docs",
        dest="html_docs",
        help="Generate HTML docs for commands, telemetry, events, and parameters",
        action="store_true",
        default=False,
    )

    parser.add_option(
        "-D",
        "--html_doc_dir",
        dest="html_doc_dir",
        help="Directory for HTML documentation",
        default=None,
    )

    parser.add_option(
        "-m",
        "--md_docs",
        dest="md_docs",
        help="Generate MarkDown docs for commands, telemetry, events, and parameters",
        action="store_true",
        default=False,
    )

    parser.add_option(
        "-M",
        "--md_doc_dir",
        dest="md_doc_dir",
        help="Directory for MarkDown documentation",
        default=None,
    )

    parser.add_option(
        "-P",
        "--is_ptr",
        dest="is_ptr",
        help="Generate component ptr's in topology.",
        action="store_true",
        default=False,
    )

    parser.add_option(
        "-C",
        "--connect_only",
        dest="connect_only",
        help="Only generate port connections in topology.",
        action="store_true",
        default=False,
    )

    parser.add_option(
        "-r",
        "--gen_report",
        dest="gen_report",
        help="Generate reports on component interfaces",
        action="store_true",
        default=False,
    )
    #    author = os.environ['USER']
    #    parser.add_option("-a", "--author", dest="author", type="string",
    #        help="Specify the new FSW author (def: %s)." % author,
    #        action="store", default=author)

    #    CONFIG = ConfigManager.ConfigManager.getInstance()
    #    v = CONFIG.get('ipc','FSW_version_id') + '_' + time.strftime("%Y%m%d")
    #    parser.add_option("-f", "--FSW_version_id", dest="fsw_ver", type="string",
    #        help="Specify the version ID here (def: %s)." % v,
    #        action="store", default=v)

    return parser


def generate_topology(the_parsed_topology_xml, xml_filename, opt):
    DEBUG.debug(f"Topology xml type description file: {xml_filename}")
    generator = TopoFactory.TopoFactory.getInstance()
    if not (opt.default_topology_dict or opt.xml_topology_dict):
        generator.set_generate_ID(False)
    topology_model = generator.create(the_parsed_topology_xml)

    if opt.is_ptr:
        PRINT.info("Topology Components will be initialized as Pointers. ")
        topology_model.is_ptr = opt.is_ptr
    if opt.connect_only:
        PRINT.info("Only port connections will be generated for Topology.")
        topology_model.connect_only = opt.connect_only

    generator = GenFactory.GenFactory.getInstance()

    if "Ai" in xml_filename:
        base = xml_filename.split("Ai")[0]
        h_instance_name = f"{base}_H"
        cpp_instance_name = f"{base}_Cpp"
        csv_instance_name = f"{base}_ID"
        cmd_html_instance_name = f"{base}_Cmd_HTML"
        channel_html_instance_name = f"{base}_Channel_HTML"
        event_html_instance_name = f"{base}_Event_HTML"
    else:
        PRINT.info("Missing Ai at end of file name...")
        raise OSError

    # Figures out what visitor to use
    if opt.default_topology_dict or opt.xml_topology_dict:
        generator.configureVisitor(
            h_instance_name, "InstanceTopologyHVisitor", True, True
        )
        generator.configureVisitor(
            cpp_instance_name, "InstanceTopologyCppVisitor", True, True
        )
    else:
        generator.configureVisitor(h_instance_name, "TopologyHVisitor", True, True)
        generator.configureVisitor(cpp_instance_name, "TopologyCppVisitor", True, True)

    # Used to generate base ID/base ID window CSV files
    generator.configureVisitor(csv_instance_name, "TopologyIDVisitor", True, True)

    # Used to generate HTML tables of ID's etc.
    if opt.default_topology_dict or opt.xml_topology_dict:
        generator.configureVisitor(
            cmd_html_instance_name, "InstanceTopologyCmdHTMLVisitor", True, True
        )
        generator.configureVisitor(
            channel_html_instance_name, "InstanceTopologyChannelsTMLVisitor", True, True
        )
        generator.configureVisitor(
            event_html_instance_name, "InstanceTopologyEventsHTMLVisitor", True, True
        )

    # uses the topology model to process the items
    if opt.default_topology_dict or opt.xml_topology_dict:
        # create list of used parsed component xmls
        parsed_xml_dict = {}
        for comp in the_parsed_topology_xml.get_instances():
            if comp.get_type() in topology_model.get_base_id_dict():
                parsed_xml_dict[comp.get_type()] = comp.get_comp_xml()
                # comp.set_component_object(comp.)
            else:
                PRINT.info(
                    f"Components with type {comp.get_type()} aren't in the topology model."
                )

        # Hack to set up deployment path for instanced dictionaries (if one exists remove old one)
        #
        if opt.default_topology_dict:
            for build_root in get_build_roots():
                if not os.path.exists(os.path.join(build_root, DEPLOYMENT)):
                    continue
                os.environ["DICT_DIR"] = os.path.join(build_root, DEPLOYMENT, "py_dict")
                break
            else:
                raise FileNotFoundError(
                    f"{DEPLOYMENT} not found in any of: {get_build_roots()}"
                )
            dict_dir = os.environ["DICT_DIR"]
            PRINT.info(f"Removing old instanced topology dictionaries in: {dict_dir}")
            import shutil

            if os.path.exists(dict_dir):
                shutil.rmtree(dict_dir)
            PRINT.info(
                f"Overriding for instanced topology dictionaries the --dict_dir option with xml derived path: {dict_dir}"
            )
        #
        xml_list = []
        for parsed_xml_type in parsed_xml_dict:
            if parsed_xml_dict[parsed_xml_type] is None:
                PRINT.info(
                    f"XML of type {parsed_xml_type} is being used, but has not been parsed correctly. Check if file exists or add xml file with the 'import_component_type' tag to the Topology file."
                )
                raise Exception()
            xml_list.append(parsed_xml_dict[parsed_xml_type])
            generate_component_instance_dictionary(
                parsed_xml_dict[parsed_xml_type], opt, topology_model
            )

        topology_model.set_instance_xml_list(xml_list)

        if opt.xml_topology_dict:
            topology_dict = etree.Element("dictionary")
            topology_dict.attrib["topology"] = the_parsed_topology_xml.get_name()
            topology_dict.attrib["framework_version"] = get_fprime_version().lstrip("v")
            topology_dict.attrib["project_version"] = get_project_version().lstrip("v")

            top_dict_gen = TopDictGenerator.TopDictGenerator(
                parsed_xml_dict, PRINT.debug
            )
            for comp in the_parsed_topology_xml.get_instances():
                comp_type = comp.get_type()
                comp_name = comp.get_name()
                comp_id = int(comp.get_base_id(), 0)
                PRINT.debug(f"Processing {comp_name} [{comp_type}] ({hex(comp_id)})")

                top_dict_gen.set_current_comp(comp)
                top_dict_gen.check_for_enum_xml()
                top_dict_gen.check_for_serial_xml()
                top_dict_gen.check_for_commands()
                top_dict_gen.check_for_channels()
                top_dict_gen.check_for_events()
                top_dict_gen.check_for_parameters()
                top_dict_gen.check_for_arrays()

            top_dict_gen.remove_duplicate_enums()

            topology_dict.append(top_dict_gen.get_enum_list())
            topology_dict.append(top_dict_gen.get_serializable_list())
            topology_dict.append(top_dict_gen.get_array_list())
            topology_dict.append(top_dict_gen.get_command_list())
            topology_dict.append(top_dict_gen.get_event_list())
            topology_dict.append(top_dict_gen.get_telemetry_list())
            topology_dict.append(top_dict_gen.get_parameter_list())

            fileName = the_parsed_topology_xml.get_xml_filename().replace(
                "Ai.xml", "Dictionary.xml"
            )
            PRINT.info(f"Generating XML dictionary {fileName}")
            fd = open(
                fileName, "wb"
            )  # Note: binary forces the same encoding of the source files
            fd.write(etree.tostring(topology_dict, pretty_print=True))

    initFiles = generator.create("initFiles")
    # startSource = generator.create("startSource")
    includes1 = generator.create("includes1")
    # includes2   = generator.create("includes2")
    # namespace   = generator.create("namespace")
    public = generator.create("public")
    finishSource = generator.create("finishSource")
    #
    # Generate the source code products here.
    #
    # 1. Open all the files
    initFiles(topology_model)
    #
    # 2. Generate includes and static code here.
    includes1(topology_model)
    #
    # 3. Generate public function to instance components and connect them here.
    public(topology_model)
    #
    # 4. Generate final code here and close all files.
    finishSource(topology_model)

    return topology_model


def generate_component_instance_dictionary(
    the_parsed_component_xml, opt, topology_model
):
    global DEPLOYMENT

    #
    parsed_port_xml_list = []
    parsed_serializable_xml_list = []
    # uses the topology model to process the items
    # checks if the topology model exists
    if topology_model is None:
        PRINT.info(
            "Topology model was not specified. Please also input a topology model when running this command."
        )
        raise OSError

    port_type_files_list = the_parsed_component_xml.get_port_type_files()

    for port_file in port_type_files_list:
        port_file = search_for_file("Port", port_file)
        xml_parser_obj = XmlPortsParser.XmlPortsParser(port_file)
        # print xml_parser_obj.get_args()
        parsed_port_xml_list.append(xml_parser_obj)
        del xml_parser_obj

    serializable_type_files_list = (
        the_parsed_component_xml.get_serializable_type_files()
    )
    for serializable_file in serializable_type_files_list:
        serializable_file = search_for_file("Serializable", serializable_file)
        xml_parser_obj = XmlSerializeParser.XmlSerializeParser(
            serializable_file
        )  # Telemetry/Params can only use generated serializable types
        # check to make sure that the serializables don't have things that channels and parameters can't have
        # can't have external non-xml members
        if len(xml_parser_obj.get_include_header_files()):
            PRINT.info(
                f"ERROR: Component include serializables cannot use user-defined types. file: {serializable_file}"
            )
            sys.exit(-1)

        # print xml_parser_obj.get_args()
        parsed_serializable_xml_list.append(xml_parser_obj)
        del xml_parser_obj

    generator = CompFactory.CompFactory.getInstance()
    component_model = generator.create(
        the_parsed_component_xml, parsed_port_xml_list, parsed_serializable_xml_list
    )

    if opt.default_topology_dict:
        default_dict_generator = GenFactory.GenFactory.getInstance()
        # iterate through command instances
        default_dict_generator.configureVisitor(
            "Commands", "InstanceCommandVisitor", True, True
        )
        for command_model in component_model.get_commands():
            DEBUG.info(f"Processing command {command_model.get_mnemonic()}")
            defaultStartCmd = default_dict_generator.create("InstanceDictStart")
            defaultCmdHeader = default_dict_generator.create("InstanceDictHeader")
            defaultCmdBody = default_dict_generator.create("InstanceDictBody")

            defaultStartCmd(command_model, topology_model)
            defaultCmdHeader(command_model, topology_model)
            defaultCmdBody(command_model, topology_model)

        for parameter_model in component_model.get_parameters():
            DEBUG.info(f"Processing parameter {parameter_model.get_name()}")
            defaultStartCmd = default_dict_generator.create("InstanceDictStart")
            defaultCmdHeader = default_dict_generator.create("InstanceDictHeader")
            defaultCmdBody = default_dict_generator.create("InstanceDictBody")

            defaultStartCmd(parameter_model, topology_model)
            defaultCmdHeader(parameter_model, topology_model)
            defaultCmdBody(parameter_model, topology_model)

        default_dict_generator = GenFactory.GenFactory.getInstance()
        # iterate through command instances
        default_dict_generator.configureVisitor(
            "Events", "InstanceEventVisitor", True, True
        )
        for event_model in component_model.get_events():
            DEBUG.info(f"Processing event {event_model.get_name()}")
            defaultStartEvent = default_dict_generator.create("InstanceDictStart")
            defaultEventHeader = default_dict_generator.create("InstanceDictHeader")
            defaultEventBody = default_dict_generator.create("InstanceDictBody")

            defaultStartEvent(event_model, topology_model)
            defaultEventHeader(event_model, topology_model)
            defaultEventBody(event_model, topology_model)

        default_dict_generator = GenFactory.GenFactory.getInstance()
        # iterate through command instances
        default_dict_generator.configureVisitor(
            "Channels", "InstanceChannelVisitor", True, True
        )
        for channel_model in component_model.get_channels():
            DEBUG.info(f"Processing channel {channel_model.get_name()}")
            defaultStartChannel = default_dict_generator.create("InstanceDictStart")
            defaultChannelHeader = default_dict_generator.create("InstanceDictHeader")
            defaultChannelBody = default_dict_generator.create("InstanceDictBody")

            defaultStartChannel(channel_model, topology_model)
            defaultChannelHeader(channel_model, topology_model)
            defaultChannelBody(channel_model, topology_model)


def generate_component(
    the_parsed_component_xml, xml_filename, opt, topology_model=None
):
    """
    Creates a component meta-model, configures visitors and
    generates the component files.  Nothing is returned.
    """
    parsed_port_xml_list = []
    if opt.gen_report:
        report_file = open(f"{xml_filename.replace('Ai.xml', '')}Report.txt", "w")
        num_input_ports = 0
        num_output_ports = 0

        # Count ports

        for port in the_parsed_component_xml.get_ports():
            if port.get_direction() == "input":
                num_input_ports = num_input_ports + int(port.get_max_number())
            if port.get_direction() == "output":
                num_output_ports = num_output_ports + int(port.get_max_number())
        if len(the_parsed_component_xml.get_ports()):
            if num_input_ports:
                report_file.write(f"Input Ports: {num_input_ports}\n")
            if num_output_ports:
                report_file.write(f"Output Ports: {num_output_ports}\n")

        # Count regular commands
        commands = 0
        idList = ""
        if len(the_parsed_component_xml.get_commands()):
            for command in the_parsed_component_xml.get_commands():
                commands += len(command.get_opcodes())
                for opcode in command.get_opcodes():
                    idList += opcode + ","

        # Count parameter commands
        if len(the_parsed_component_xml.get_parameters()):
            for parameter in the_parsed_component_xml.get_parameters():
                commands += len(parameter.get_set_opcodes())
                for opcode in parameter.get_set_opcodes():
                    idList += opcode + ","
                commands += len(parameter.get_save_opcodes())
                for opcode in parameter.get_save_opcodes():
                    idList += opcode + ","

        if commands > 0:
            report_file.write(f"Commands: {commands}\n OpCodes: {idList[:-1]}\n")

        if len(the_parsed_component_xml.get_channels()):
            idList = ""
            channels = 0
            for channel in the_parsed_component_xml.get_channels():
                channels += len(channel.get_ids())
                for id in channel.get_ids():
                    idList += id + ","
            report_file.write(f"Channels: {channels}\n ChanIds: {idList[:-1]}\n")

        if len(the_parsed_component_xml.get_events()):
            idList = ""
            events = 0
            for event in the_parsed_component_xml.get_events():
                events += len(event.get_ids())
                for id in event.get_ids():
                    idList += id + ","
            report_file.write(f"Events: {events}\n EventIds: {idList[:-1]}\n")

        if len(the_parsed_component_xml.get_parameters()):
            idList = ""
            parameters = 0
            for parameter in the_parsed_component_xml.get_parameters():
                parameters += len(parameter.get_ids())
                for id in parameter.get_ids():
                    idList += id + ","
            report_file.write(f"Parameters: {parameters}\n ParamIds: {idList[:-1]}\n")
    #
    # Configure the meta-model for the component
    #
    port_type_files_list = the_parsed_component_xml.get_port_type_files()

    for port_file in port_type_files_list:
        port_file = search_for_file("Port", port_file)
        xml_parser_obj = XmlPortsParser.XmlPortsParser(port_file)
        # print xml_parser_obj.get_args()
        parsed_port_xml_list.append(xml_parser_obj)
        del xml_parser_obj

    parsed_serializable_xml_list = []
    #
    # Configure the meta-model for the component
    #
    serializable_type_files_list = (
        the_parsed_component_xml.get_serializable_type_files()
    )
    for serializable_file in serializable_type_files_list:
        serializable_file = search_for_file("Serializable", serializable_file)
        xml_parser_obj = XmlSerializeParser.XmlSerializeParser(
            serializable_file
        )  # Telemetry/Params can only use generated serializable types
        # check to make sure that the serializables don't have things that channels and parameters can't have
        # can't have external non-xml members
        if len(xml_parser_obj.get_include_header_files()):
            PRINT.info(
                f"ERROR: Component include serializables cannot use user-defined types. file: {serializable_file}"
            )
            sys.exit(-1)

        # print xml_parser_obj.get_args()
        parsed_serializable_xml_list.append(xml_parser_obj)
        del xml_parser_obj

    #
    # for p in the_parsed_component_xml.get_ports():
    #    print p.get_name(), p.get_type()
    # print parsed_port_xml_list

    # for p in parsed_port_xml_list:
    #    print p.get_interface().get_name(), p.get_interface().get_namespace()
    #    print p.get_args()
    #    print p.get_include_header_files()
    #
    generator = CompFactory.CompFactory.getInstance()
    component_model = generator.create(
        the_parsed_component_xml, parsed_port_xml_list, parsed_serializable_xml_list
    )

    # tv = [x for x in component_model.get_events()]
    # for event_model in component_model.get_events():
    #    event_model.set_ids([1,2,3])
    #    tv.append(event_model)

    #
    # Configure and create the visitors that will generate the code.
    #
    generator = GenFactory.GenFactory.getInstance()
    #
    # Configure each visitor here.
    #
    if "Ai" in xml_filename:
        base = xml_filename.split("Ai")[0]
        h_instance_name = base + "_H"
        cpp_instance_name = base + "_Cpp"
        h_instance_name_tmpl = base + "_Impl_H"
        cpp_instance_name_tmpl = base + "_Impl_Cpp"
        h_instance_test_name = base + "_Test_H"
        cpp_instance_test_name = base + "_Test_Cpp"
        h_instance_gtest_name = base + "_GTest_H"
        cpp_instance_gtest_name = base + "_GTest_Cpp"
        h_instance_test_impl_name = base + "_TestImpl_H"
        cpp_instance_test_impl_name = base + "_TestImpl_Cpp"
        cpp_instance_test_impl_helpers_name = base + "_TestImplHelpers_Cpp"
        test_main_name = base + "_TestMain_Cpp"
    else:
        PRINT.info("Missing Ai at end of file name...")
        raise OSError

    #
    if opt.impl_flag:
        PRINT.info("Enabled generation of implementation template files...")
        generator.configureVisitor(h_instance_name_tmpl, "ImplHVisitor", True, True)
        generator.configureVisitor(cpp_instance_name_tmpl, "ImplCppVisitor", True, True)
    elif opt.unit_test:
        PRINT.info("Enabled generation of unit test component files...")
        generator.configureVisitor(
            h_instance_test_name, "ComponentTestHVisitor", True, True
        )
        generator.configureVisitor(
            cpp_instance_test_name, "ComponentTestCppVisitor", True, True
        )
        generator.configureVisitor(h_instance_gtest_name, "GTestHVisitor", True, True)
        generator.configureVisitor(
            cpp_instance_gtest_name, "GTestCppVisitor", True, True
        )
        generator.configureVisitor(
            h_instance_test_impl_name, "TestImplHVisitor", True, True
        )
        generator.configureVisitor(
            cpp_instance_test_impl_name, "TestImplCppVisitor", True, True
        )
        generator.configureVisitor(
            cpp_instance_test_impl_helpers_name, "TestImplCppHelpersVisitor", True, True
        )
        generator.configureVisitor(test_main_name, "TestMainVisitor", True, True)
    else:
        generator.configureVisitor(h_instance_name, "ComponentHVisitor", True, True)
        generator.configureVisitor(cpp_instance_name, "ComponentCppVisitor", True, True)

    # for port_file in port_type_files_list:
    #    if "Ai" in port_file:
    #        base = port_file.split("Ai")[0]
    #        h_instance_name = base + "_H"
    #        cpp_instance_name = base + "_Cpp"
    #    else:
    #        PRINT.info("Missing Ai at end of file: %s" % port_file)
    #        raise IOError
    #    generator.configureVisitor(h_instance_name, "PortCppVisitor", True, True)
    #    generator.configureVisitor(cpp_instance_name, "PortHVisitor", True, True)
    #
    # The idea here is that each of these generators is used to create
    # a certain portion of each output file.
    #
    initFiles = generator.create("initFiles")
    startSource = generator.create("startSource")
    includes1 = generator.create("includes1")
    includes2 = generator.create("includes2")
    namespace = generator.create("namespace")
    public = generator.create("public")
    protected = generator.create("protected")
    private = generator.create("private")
    finishSource = generator.create("finishSource")

    #
    # Generate the source code products here.
    #
    # 1. Open all the files
    initFiles(component_model)
    #
    # 2. Produce caltech notice here and other starting stuff.
    startSource(component_model)
    #
    # 3. Generate includes that all files get here.
    includes1(component_model)
    #
    # 4. Generate includes from model that a specific here.
    includes2(component_model)
    #
    # 5. Generate start of namespace here.
    namespace(component_model)
    #
    # 6. Generate public class code here.
    public(component_model)
    #
    # 7. Generate protected class code here.
    protected(component_model)
    #
    # 8. Generate private class code here.
    private(component_model)
    #
    # 9. Generate final code here and close all files.
    finishSource(component_model)
    #

    # if requested, generate ground system dictionary
    if opt.default_dict:
        if opt.dict_dir is None:
            PRINT.info("Dictionary output directory not specified!")
            raise OSError
        os.environ["DICT_DIR"] = opt.dict_dir
        default_dict_generator = GenFactory.GenFactory.getInstance()
        # iterate through command instances
        default_dict_generator.configureVisitor(
            "Commands", "CommandVisitor", True, True
        )
        for command_model in component_model.get_commands():
            DEBUG.info(f"Processing command {command_model.get_mnemonic()}")
            defaultStartCmd = default_dict_generator.create("DictStart")
            defaultCmdHeader = default_dict_generator.create("DictHeader")
            defaultCmdBody = default_dict_generator.create("DictBody")

            defaultStartCmd(command_model)
            defaultCmdHeader(command_model)
            defaultCmdBody(command_model)

        for parameter_model in component_model.get_parameters():
            DEBUG.info(f"Processing parameter {parameter_model.get_name()}")
            defaultStartCmd = default_dict_generator.create("DictStart")
            defaultCmdHeader = default_dict_generator.create("DictHeader")
            defaultCmdBody = default_dict_generator.create("DictBody")

            defaultStartCmd(parameter_model)
            defaultCmdHeader(parameter_model)
            defaultCmdBody(parameter_model)

        default_dict_generator = GenFactory.GenFactory.getInstance()
        # iterate through command instances
        default_dict_generator.configureVisitor("Events", "EventVisitor", True, True)
        for event_model in component_model.get_events():
            DEBUG.info(f"Processing event {event_model.get_name()}")
            defaultStartEvent = default_dict_generator.create("DictStart")
            defaultEventHeader = default_dict_generator.create("DictHeader")
            defaultEventBody = default_dict_generator.create("DictBody")

            defaultStartEvent(event_model)
            defaultEventHeader(event_model)
            defaultEventBody(event_model)

        default_dict_generator = GenFactory.GenFactory.getInstance()
        # iterate through command instances
        default_dict_generator.configureVisitor(
            "Channels", "ChannelVisitor", True, True
        )
        for channel_model in component_model.get_channels():
            DEBUG.info(f"Processing channel {channel_model.get_name()}")
            defaultStartChannel = default_dict_generator.create("DictStart")
            defaultChannelHeader = default_dict_generator.create("DictHeader")
            defaultChannelBody = default_dict_generator.create("DictBody")

            defaultStartChannel(channel_model)
            defaultChannelHeader(channel_model)
            defaultChannelBody(channel_model)

    if opt.html_docs:
        if opt.html_doc_dir is None:
            PRINT.info("HTML documentation output directory not specified!")
            raise OSError

        os.environ["HTML_DOC_SUBDIR"] = opt.html_doc_dir
        html_doc_generator = GenFactory.GenFactory.getInstance()
        html_doc_generator.configureVisitor(
            base + "_Html", "HtmlDocVisitor", True, True
        )
        htmlStart = html_doc_generator.create("HtmlStart")
        htmlDoc = html_doc_generator.create("HtmlDoc")
        finisher = html_doc_generator.create("finishSource")
        htmlStart(component_model)
        htmlDoc(component_model)
        finisher(component_model)

    if opt.md_docs:
        if opt.md_doc_dir is None:
            PRINT.info("MD documentation output directory not specified!")
            raise OSError

        os.environ["MD_DOC_SUBDIR"] = opt.md_doc_dir
        md_doc_generator = GenFactory.GenFactory.getInstance()
        md_doc_generator.configureVisitor(base + "_Md", "MdDocVisitor", True, True)
        mdStart = md_doc_generator.create("MdStart")
        mdDoc = md_doc_generator.create("MdDoc")
        finisher = md_doc_generator.create("finishSource")
        mdStart(component_model)
        mdDoc(component_model)
        finisher(component_model)


def generate_port(the_parsed_port_xml, port_file):
    """
    Creates a port meta-model, configures visitors and
    generates the port/interface type files.  Nothing is returned.
    """
    #
    # Configure the meta-model for the component
    #
    DEBUG.debug(f"Port xml type description file: {port_file}")
    generator = PortFactory.PortFactory.getInstance()
    port_model = generator.create(the_parsed_port_xml)
    #
    # Configure and create the visitors that will generate the code.
    #
    generator = GenFactory.GenFactory.getInstance()
    #
    # Configure file names and each visitor here.
    #
    the_type = the_parsed_port_xml.get_interface().get_name()
    #
    # Configure each visitor here.
    #
    if "Ai" in port_file:
        base = the_type
        h_instance_name = base + "_H"
        cpp_instance_name = base + "_Cpp"
    else:
        PRINT.info("Missing Ai at end of file name...")
        raise OSError
    #
    generator.configureVisitor(h_instance_name, "PortCppVisitor", True, True)
    generator.configureVisitor(cpp_instance_name, "PortHVisitor", True, True)
    #
    # The idea here is that each of these generators is used to create
    # a certain portion of each output file.
    #
    initFiles = generator.create("initFiles")
    startSource = generator.create("startSource")
    includes1 = generator.create("includes1")
    includes2 = generator.create("includes2")
    namespace = generator.create("namespace")
    public = generator.create("public")
    protected = generator.create("protected")
    private = generator.create("private")
    finishSource = generator.create("finishSource")
    #
    # Generate the source code products here.
    #
    # 1. Open all the files
    initFiles(port_model)
    #
    # 2. Produce caltech notice here and other starting stuff.
    startSource(port_model)
    #
    # 3. Generate includes that all files get here.
    includes1(port_model)
    #
    # 4. Generate includes from model that a specific here.
    includes2(port_model)
    #
    # 5. Generate start of namespace here.
    namespace(port_model)
    #
    # 6. Generate public class code here.
    public(port_model)
    #
    # 7. Generate protected class code here.
    protected(port_model)
    #
    # 8. Generate private class code here.
    private(port_model)
    #
    # 9. Generate final code here and close all files.
    finishSource(port_model)


def generate_serializable(the_serial_xml, opt):
    """
    Creates a serializable meta-model class, configures visitors and
    generates the serializable class files.  Nothing is returned.
    """
    #
    # Configure the meta-model for the serializable here
    #
    f = the_serial_xml.get_xml_filename()
    DEBUG.debug(f"Serializable xml type description file: {f}")
    n = the_serial_xml.get_name()
    ns = the_serial_xml.get_namespace()
    c = the_serial_xml.get_comment()
    i = (
        the_serial_xml.get_includes()
        + the_serial_xml.get_include_enums()
        + the_serial_xml.get_include_arrays()
    )
    i2 = the_serial_xml.get_include_header_files()
    m = the_serial_xml.get_members()
    t = the_serial_xml.get_typeid()
    model = Serialize.Serialize(f, n, ns, c, i, i2, m, t)
    #
    # Configure each visitor here.
    #
    t = f.split(".")[0][-2:]
    if ("Ai" in f) & (t == "Ai"):
        base = n
        h_instance_name = base + "_H"
        cpp_instance_name = base + "_Cpp"
    else:
        PRINT.info("Missing Ai at end of file name...")
        raise OSError
    #
    generator = GenFactory.GenFactory.getInstance()
    generator.configureVisitor(h_instance_name, "SerialCppVisitor", True, True)
    generator.configureVisitor(cpp_instance_name, "SerialHVisitor", True, True)
    # only generate if serializable is usable for dictionary. Can't have includes of other types
    if opt.default_dict:
        if len(i) != 0 or len(i2) != 0:
            PRINT.info(f"Dictionary: Skipping {f} because of external includes")
        else:
            # borrow source visitor pattern for serializable dictionary
            if opt.dict_dir is None:
                PRINT.info("Dictionary output directory not specified!")
                raise OSError
            os.environ["DICT_DIR"] = opt.dict_dir
            generator.configureVisitor("SerialDict", "SerializableVisitor", True, True)

    if opt.default_topology_dict:
        if len(i) != 0 or len(i2) != 0:
            PRINT.info(f"Dictionary: Skipping {f} because of external includes")
        else:
            # borrow source visitor pattern for serializable dictionary
            if opt.dict_dir is None:
                PRINT.info("Dictionary output directory not specified!")
                raise OSError
            os.environ["DICT_DIR"] = opt.dict_dir
            print("\n")
            print(opt.dict_dir)
            print("\n")
            generator.configureVisitor(
                "SerialDict", "InstanceSerializableVisitor", True, True
            )
    #
    # The idea here is that each of these generators is used to create
    # a certain portion of each output file.
    #
    initFiles = generator.create("initFiles")
    startSource = generator.create("startSource")
    includes1 = generator.create("includes1")
    includes2 = generator.create("includes2")
    namespace = generator.create("namespace")
    public = generator.create("public")
    protected = generator.create("protected")
    private = generator.create("private")
    finishSource = generator.create("finishSource")
    #
    # Generate the source code products here.
    #
    # 1. Open all the files
    initFiles(model)
    #
    # 2. Produce caltech notice here and other starting stuff.
    startSource(model)
    #
    # 3. Generate includes that all files get here.
    includes1(model)
    #
    # 4. Generate includes from model that a specific here.
    includes2(model)
    #
    # 5. Generate start of namespace here.
    namespace(model)
    #
    # 6. Generate public class code here.
    public(model)
    #
    # 7. Generate protected class code here.
    protected(model)
    #
    # 8. Generate private class code here.
    private(model)
    #
    # 9. Generate final code here and close all files.
    finishSource(model)


def generate_dependency_file(filename, target_file, subst_path, parser, the_type):

    # verify directory exists for dependency file and is directory
    if not os.path.isdir(os.path.dirname(filename)):
        PRINT.info(
            f"ERROR: Dependency file path {os.path.dirname(filename)} does not exist!"
        )
        sys.exit(-1)

    # open dependency file
    dep_file = open(filename, "w")
    # get working directory and normalize path
    target_directory = os.getcwd().replace("\\", "/")
    target_file_local = target_file.replace("\\", "/").replace("Ai.xml", "Ac.cpp")
    subst_path_local = subst_path.replace("\\", "/")

    # normalize path to target file
    full_path = os.path.abspath(target_directory + "/" + target_file_local).replace(
        "\\", "/"
    )
    # if path to substitute is specified, replace with build root
    if subst_path_local is not None:
        full_path = full_path.replace(subst_path_local, "$(BUILD_ROOT)")

    # print("sub: %s\ndep_file: %s\ntdir: %s\ntfile: %s\nfp: %s"%(subst_path_local,filename,target_directory,target_file_local,full_path))

    # write target to file
    dep_file.write(f"{full_path}:")

    # assemble list of files

    if the_type == "interface":
        file_list = (
            parser.get_include_header_files()
            + parser.get_includes_serial_files()
            + parser.get_include_enum_files()
            + parser.get_include_array_files()
        )
    elif the_type == "component":
        file_list = (
            parser.get_port_type_files()
            + parser.get_header_files()
            + parser.get_serializable_type_files()
            + parser.get_imported_dictionary_files()
            + parser.get_enum_type_files()
            + parser.get_array_type_files()
        )
    elif the_type == "serializable":
        file_list = (
            parser.get_include_header_files()
            + parser.get_includes()
            + parser.get_include_enums()
            + parser.get_include_arrays()
        )
    elif the_type in ("assembly", "deployment"):
        # get list of dependency files from XML/header file list
        file_list_tmp = list(parser.get_comp_type_file_header_dict().keys())
        file_list = file_list_tmp
        # file_list = []
        # for f in file_list_tmp:
        #    file_list.append(f.replace("Ai.xml","Ac.hpp"))
    else:
        PRINT.info(f"ERROR: Unrecognized dependency type {the_type}!")
        sys.exit(-1)

    # write dependencies
    for include in file_list:
        # print("include %s\n"%include)
        if subst_path_local is not None:
            full_path = "$(BUILD_ROOT)/" + include.replace("\\", "/")
        else:
            PRINT.info(
                "ERROR: No build root to attach. Not sure how to generate dependency."
            )
            sys.exit(-1)

        dep_file.write(f"\\\n    {full_path}  ")

    # carriage return
    dep_file.write("\n\n")
    # close file
    dep_file.close()


def main():
    """
    Main program.
    """
    global ERROR  # prevent local creation of variable
    global VERBOSE  # prevent local creation of variable
    global GEN_TEST_CODE  # indicate if test code should be generated
    global DEPLOYMENT  # deployment set in topology xml only and used to install new instance dicts

    ERROR = False
    # Sets up the initial (singleton) instance
    ConfigManager.ConfigManager.getInstance()
    Parser = pinit()
    (opt, args) = Parser.parse_args()
    VERBOSE = opt.verbose_flag

    # Check that the specified working directory exists. Remember, the
    # default working directory is the current working directory which
    # always exists. We are basically only checking for when the user
    # specifies an alternate working directory.

    if not os.path.exists(opt.work_path):
        Parser.error(f"Specified path does not exist ({opt.work_path})!")

    working_dir = opt.work_path

    # Get the current working directory so that we can return to it when
    # the program completes. We always want to return to the place where
    # we started.

    starting_directory = os.getcwd()
    os.chdir(working_dir)
    # print working_dir
    # print os.getcwd()

    # Configure the logging.
    log_level = opt.logger.upper()
    log_level_dict = {
        "QUIET": None,
        "DEBUG": logging.DEBUG,
        "INFO": logging.INFO,
        "WARNING": logging.WARN,
        "ERROR": logging.ERROR,
        "CRITICAL": logging.CRITICAL,
    }

    if log_level_dict[log_level] is None:
        stdout_enable = False
    else:
        stdout_enable = True

    log_fd = opt.logger_output
    # For now no log file

    Logger.connectDebugLogger(log_level_dict[log_level], log_fd, stdout_enable)
    Logger.connectOutputLogger(log_fd)
    #
    #  Parse the input Component XML file and create internal meta-model
    #
    if len(args) == 0:
        PRINT.info(f"Usage: {sys.argv[0]} [options] xml_filename")
        return
    else:
        xml_filenames = args[0:]
    #
    # Check for BUILD_ROOT variable for XML port searches
    #
    if opt.build_root_flag:
        # Check for BUILD_ROOT env. variable
        if not ("BUILD_ROOT" in list(os.environ.keys())):
            PRINT.info(
                "ERROR: The -b command option requires that BUILD_ROOT environmental variable be set to root build path..."
            )
            sys.exit(-1)
        else:
            set_build_roots(os.environ.get("BUILD_ROOT"))

    for xml_filename in xml_filenames:

        xml_type = XmlParser.XmlParser(xml_filename)()

        if xml_type == "component":
            DEBUG.info("Detected Component XML so Generating Component C++ Files...")
            the_parsed_component_xml = XmlComponentParser.XmlComponentParser(
                xml_filename
            )
            generate_component(
                the_parsed_component_xml, os.path.basename(xml_filename), opt
            )
            dependency_parser = the_parsed_component_xml
        elif xml_type == "interface":
            DEBUG.info("Detected Port type XML so Generating Port type C++ Files...")
            the_parsed_port_xml = XmlPortsParser.XmlPortsParser(xml_filename)
            generate_port(the_parsed_port_xml, os.path.basename(xml_filename))
            dependency_parser = the_parsed_port_xml
        elif xml_type == "serializable":
            DEBUG.info(
                "Detected Serializable XML so Generating Serializable C++ Files..."
            )
            the_serial_xml = XmlSerializeParser.XmlSerializeParser(xml_filename)
            generate_serializable(the_serial_xml, opt)
            dependency_parser = the_serial_xml
        elif xml_type in ("assembly", "deployment"):
            DEBUG.info("Detected Topology XML so Generating Topology C++ Files...")
            the_parsed_topology_xml = XmlTopologyParser.XmlTopologyParser(xml_filename)
            DEPLOYMENT = the_parsed_topology_xml.get_deployment()
            print("Found assembly or deployment named: %s\n" % DEPLOYMENT)
            generate_topology(
                the_parsed_topology_xml, os.path.basename(xml_filename), opt
            )
            dependency_parser = the_parsed_topology_xml
        elif xml_type == "enum":
            DEBUG.info("Detected Enum XML so Generating hpp, cpp, and py files...")
            curdir = os.getcwd()
            if EnumGenerator.generate_enum(xml_filename):
                ERROR = False
                PRINT.info(
                    f"Completed generating files for {xml_filename} Enum XML...."
                )
            else:
                ERROR = True
            os.chdir(curdir)
        elif xml_type == "array":
            DEBUG.info("Detected Array XML so Generating hpp, cpp, and py files...")
            curdir = os.getcwd()
            if ArrayGenerator.generate_array(xml_filename):
                ERROR = False
                PRINT.info(
                    f"Completed generating files for {xml_filename} Array XML..."
                )
            else:
                ERROR = True
            os.chdir(curdir)
        else:
            PRINT.info("Invalid XML found...this format not supported")
            ERROR = True

        if opt.dependency_file is not None:
            if opt.build_root_flag:
                generate_dependency_file(
                    opt.dependency_file,
                    os.path.basename(xml_filename),
                    list(get_build_roots())[0],
                    dependency_parser,
                    xml_type,
                )

    # Always return to directory where we started.
    os.chdir(starting_directory)

    if ERROR:
        sys.exit(-1)
    else:
        sys.exit(0)


if __name__ == "__main__":
    try:
        main()
    except Exception as exc:
        print(exc, file=sys.stderr)
        traceback.print_exc(file=sys.stdout)
        sys.exit(-1)
