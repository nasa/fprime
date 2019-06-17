#!/usr/bin/env python
#===============================================================================
# NAME: codegen.py
#
# DESCRIPTION: This script is used to generate components, ports and connectors
# from XML definition files.
#
#===============================================================================

import os
import sys
import time
import glob
import logging


from optparse import OptionParser

from fprime_ac.utils import Logger
from fprime_ac.utils import ConfigManager
from fprime_ac.utils import DictTypeConverter


# Meta-model for Component only generation
from fprime_ac.models import CompFactory
from fprime_ac.models import PortFactory
from fprime_ac.models import TopoFactory
from fprime_ac.models import Serialize
from fprime_ac.models import ModelParser


# Parsers to read the XML
from fprime_ac.parsers import XmlParser
from fprime_ac.parsers import XmlComponentParser
from fprime_ac.parsers import XmlPortsParser
from fprime_ac.parsers import XmlSerializeParser
from fprime_ac.parsers import XmlTopologyParser

from lxml import etree

#Generators to produce the code
from fprime_ac.generators import GenFactory

# Flag to indicate verbose mode.
VERBOSE = False

# Global logger init. below.
PRINT = logging.getLogger('output')
DEBUG = logging.getLogger('debug')

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

# Build Root environmental variable if one exists.
BUILD_ROOT = None

# Deployment name from topology XML only
DEPLOYMENT = None

# Version label for now
class Version:
    id      = "0.1"
    comment = "Initial prototype"
VERSION = Version()

def moveCAndHFiles(path_prefix):
    """
    Moves the generated c and h files
    to a specific directory.
    """
    for f in glob.glob('*.c'):
        fp = path_prefix + os.sep + f
        os.rename(f,fp)
    for f in glob.glob('*.h'):
        fp = path_prefix + os.sep + f
        os.rename(f,fp)

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
    vers = "%prog " + VERSION.id + " " + VERSION.comment

    parser = OptionParser(usage, version=vers)

    parser.add_option("-b", "--build_root", dest="build_root_flag",
        help="Enable search for enviornment variable BUILD_ROOT to establish absolute XML directory path",
        action="store_true", default=False)

    parser.add_option("-p", "--path", dest="work_path", type="string",
        help="Switch to new working directory (def: %s)." % current_dir,
        action="store", default=current_dir)

    parser.add_option("-v", "--verbose", dest="verbose_flag",
        help="Enable verbose mode showing more runtime detail (def: False)",
        action="store_true", default=False)

    parser.add_option("-t", "--template", dest="impl_flag",
        help="Enable generation of *Impl_[hpp,cpp].template implementation template files (def: False)",
        action="store_true", default=False)

    parser.add_option("-u", "--unit-test", dest="unit_test",
        help="Enable generation of unit test component files (def: False)",
        action="store_true", default=False)

    parser.add_option("-l", "--logger", dest="logger", default="QUIET",
        help="Set the logging level <DEBUG | INFO | QUIET> (def: 'QUIET').")

    parser.add_option("-L", "--logger-output-file", dest="logger_output",
        default=None, help="Set the logger output file. (def: defaultgen.log).")

    parser.add_option("-H", "--html_docs", dest="html_docs",
        help="Generate HTML docs for commands, telemetry, events, and parameters", action="store_true", default=False)

    parser.add_option("-D", "--html_doc_dir", dest="html_doc_dir",
        help="Directory for HTML documentation", default=None)

    parser.add_option("-m", "--md_docs", dest="md_docs",
        help="Generate MarkDown docs for commands, telemetry, events, and parameters", action="store_true", default=False)

    parser.add_option("-M", "--md_doc_dir", dest="md_doc_dir",
        help="Directory for MarkDown documentation", default=None)

    parser.add_option("-P", "--is_ptr", dest="is_ptr",
        help="Generate component ptr's in topology.", action="store_true", default=False)

    parser.add_option("-C", "--connect_only", dest="connect_only",
        help="Only generate port connections in topology.", action="store_true", default=False)

    parser.add_option("-r", "--gen_report", dest="gen_report",
        help="Generate reports on component interfaces", action="store_true", default=False)

    return parser

def generate_component(the_parsed_component_xml, xml_filename, opt , topology_model = None):
    """
    Creates a component meta-model, configures visitors and
    generates the component files.  Nothing is returned.
    """
    global BUILD_ROOT
    #

    parsed_port_xml_list = []
    if opt.gen_report:
        report_file = open("%sReport.txt"%xml_filename.replace("Ai.xml",""),"w")
        num_input_ports = 0
        num_output_ports = 0

        # Count ports

        for port in the_parsed_component_xml.get_ports():
            if port.get_direction() == "input":
                num_input_ports = num_input_ports + int(port.get_max_number())
            if port.get_direction() == "output":
                num_output_ports = num_output_ports + int(port.get_max_number())
        if len(the_parsed_component_xml.get_ports()):
            if (num_input_ports):
                report_file.write("Input Ports: %d\n"%num_input_ports)
            if (num_output_ports):
                report_file.write("Output Ports: %d\n"%num_output_ports)

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
            report_file.write("Commands: %d\n OpCodes: %s\n"%(commands,idList[:-1]))

        if len(the_parsed_component_xml.get_channels()):
            idList = ""
            channels = 0
            for channel in the_parsed_component_xml.get_channels():
                channels += len(channel.get_ids())
                for id in channel.get_ids():
                    idList += id + ","
            report_file.write("Channels: %d\n ChanIds: %s\n"%(channels,idList[:-1]))

        if len(the_parsed_component_xml.get_events()):
            idList = ""
            events = 0
            for event in the_parsed_component_xml.get_events():
                events += len(event.get_ids())
                for id in event.get_ids():
                    idList += id + ","
            report_file.write("Events: %d\n EventIds: %s\n"%(events,idList[:-1]))

        if len(the_parsed_component_xml.get_parameters()):
            idList = ""
            parameters = 0
            for parameter in the_parsed_component_xml.get_parameters():
                parameters += len(parameter.get_ids())
                for id in parameter.get_ids():
                    idList += id + ","
            report_file.write("Parameters: %d\n ParamIds: %s\n"%(parameters,idList[:-1]))
    #
    # Configure the meta-model for the component
    #
    port_type_files_list = the_parsed_component_xml.get_port_type_files()

    for port_file in port_type_files_list:
        port_file = search_for_file("Port", port_file)
        xml_parser_obj = XmlPortsParser.XmlPortsParser(port_file)
        #print xml_parser_obj.get_args()
        parsed_port_xml_list.append(xml_parser_obj)
        del(xml_parser_obj)

    parsed_serializable_xml_list = []
    #
    # Configure the meta-model for the component
    #
    serializable_type_files_list = the_parsed_component_xml.get_serializable_type_files()
    for serializable_file in serializable_type_files_list:
        serializable_file = search_for_file("Serializable", serializable_file)
        xml_parser_obj = XmlSerializeParser.XmlSerializeParser(serializable_file) # Telemetry/Params can only use generated serializable types
        # check to make sure that the serializables don't have things that channels and parameters can't have
        # can't have external non-xml members
        if len(xml_parser_obj.get_include_header_files()):
            PRINT.info("ERROR: Component include serializables cannot use user-defined types. file: " % serializable_file)
            sys.exit(-1)

        #print xml_parser_obj.get_args()
        parsed_serializable_xml_list.append(xml_parser_obj)
        del(xml_parser_obj)

    generator = CompFactory.CompFactory.getInstance()
    component_model = generator.create(the_parsed_component_xml, parsed_port_xml_list, parsed_serializable_xml_list)

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
    else:
        PRINT.info("Missing Ai at end of file name...")
        raise IOError

    #
    if opt.impl_flag:
        PRINT.info("Enabled generation of implementation template files...")
        generator.configureVisitor(h_instance_name_tmpl, "ImplHVisitor", True, True)
        generator.configureVisitor(cpp_instance_name_tmpl, "ImplCppVisitor", True, True)
    elif opt.unit_test:
        PRINT.info("Enabled generation of unit test component files...")
        generator.configureVisitor(h_instance_test_name, "ComponentTestHVisitor", True, True)
        generator.configureVisitor(cpp_instance_test_name, "ComponentTestCppVisitor", True, True)
        generator.configureVisitor(h_instance_gtest_name, "GTestHVisitor", True, True)
        generator.configureVisitor(cpp_instance_gtest_name, "GTestCppVisitor", True, True)
        generator.configureVisitor(h_instance_test_impl_name, "TestImplHVisitor", True, True)
        generator.configureVisitor(cpp_instance_test_impl_name, "TestImplCppVisitor", True, True)
    else:
        generator.configureVisitor(h_instance_name, "ComponentHVisitor", True, True)
        generator.configureVisitor(cpp_instance_name, "ComponentCppVisitor", True, True)

    # The idea here is that each of these generators is used to create
    # a certain portion of each output file.
    #
    initFiles   = generator.create("initFiles")
    startSource = generator.create("startSource")
    includes1   = generator.create("includes1")
    includes2   = generator.create("includes2")
    namespace   = generator.create("namespace")
    public      = generator.create("public")
    protected   = generator.create("protected")
    private     = generator.create("private")
    finishSource= generator.create("finishSource")

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

def search_for_file(file_type, file_path):
    '''
    Searches for a given included port or serializable by looking in three places:
     - The specified BUILD_ROOT
     - The F Prime core
     - The exact specified path
    @param file_type: type of file searched for
    @param file_path: path to look for based on offset
    @return: full path of file
    '''
    core = os.environ.get("FPRIME_CORE_DIR", BUILD_ROOT)
    for possible in [BUILD_ROOT, core, None]:
        if not possible is None:
            checker = os.path.join(possible, file_path)
        else:
            checker = file_path
        if os.path.exists(checker):
            DEBUG.debug("%s xml type description file: %s" % (file_type,file_path))
            return checker
    else:
        PRINT.info("ERROR: %s xml specification file %s does not exist!" % (file_type,file_path))
        sys.exit(-1)

def main():
    """
    Main program.
    """
    global ERROR   # prevent local creation of variable
    global VERBOSE # prevent local creation of variable
    global BUILD_ROOT # environmental variable if set
    global GEN_TEST_CODE # indicate if test code should be generated
    global DEPLOYMENT # deployment set in topology xml only and used to install new instance dicts

    ERROR = False
    CONFIG = ConfigManager.ConfigManager.getInstance()
    Parser = pinit()
    (opt, args) = Parser.parse_args()
    VERBOSE = opt.verbose_flag

    # Check that the specified working directory exists. Remember, the
    # default working directory is the current working directory which
    # always exists. We are basically only checking for when the user
    # specifies an alternate working directory.

    if os.path.exists(opt.work_path) == False:
        Parser.error('Specified path does not exist (%s)!' % opt.work_path)

    working_dir = opt.work_path

    # Get the current working directory so that we can return to it when
    # the program completes. We always want to return to the place where
    # we started.

    starting_directory = os.getcwd()
    os.chdir(working_dir)
    #print working_dir
    #print os.getcwd()

    # Configure the logging.
    log_level = opt.logger.upper()
    log_level_dict = dict()

    log_level_dict['QUIET']    = None
    log_level_dict['DEBUG']    = logging.DEBUG
    log_level_dict['INFO']     = logging.INFO
    log_level_dict['WARNING']  = logging.WARN
    log_level_dict['ERROR']    = logging.ERROR
    log_level_dict['CRITICAL'] = logging.CRITICAL

    if log_level_dict[log_level] == None:
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
        PRINT.info("Usage: %s [options] xml_filename" % sys.argv[0])
        return
    else:
        xml_filenames = args[0:]
    #
    # Check for BUILD_ROOT variable for XML port searches
    #
    if opt.build_root_flag == True:
        # Check for BUILD_ROOT env. variable
        if ('BUILD_ROOT' in list(os.environ.keys())) == False:
            PRINT.info("ERROR: The -b command option requires that BUILD_ROOT environmental variable be set to root build path...")
            sys.exit(-1)
        else:
            BUILD_ROOT = os.environ['BUILD_ROOT']
            ModelParser.BUILD_ROOT = BUILD_ROOT
            #PRINT.info("BUILD_ROOT set to %s"%BUILD_ROOT)

    for xml_filename in xml_filenames:

        xml_filename = os.path.basename(xml_filename)
        xml_type = XmlParser.XmlParser(xml_filename)()

        if xml_type == "component":
            DEBUG.info("Detected Component XML so Generating Component C++ Files...")
            the_parsed_component_xml = XmlComponentParser.XmlComponentParser(xml_filename)
            generate_component(the_parsed_component_xml, xml_filename, opt)
            dependency_parser = the_parsed_component_xml
        else:
            PRINT.info("Invalid XML found...this format not supported")
            ERROR=True

    # Always return to directory where we started.
    os.chdir(starting_directory)

    if ERROR == True:
        sys.exit(-1)
    else:
        sys.exit(0)


if __name__ == '__main__':
    main()
