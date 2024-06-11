#
# ===============================================================================
# NAME: ConfigManager.py
#
# DESCRIPTION: This is a simple configuration class patterned after the
#       one in the Keck observation sequencer GUI and the Tahoe
#       CalVal pipeline.  The configuration class has changed since
#       previous ones.  This new one uses the ConfigParser module
#       included with Python 2.4 to extend configuration out to
#       reading windows like .ini files.  If none exist then this
#       uses hardwired values set in a dictionary called prop to
#       default initialization.
#
# AUTHOR: Leonard J. Reder
#
# EMAIL:  reder@jpl.nasa.gov
#
# DATE CREATED  : 30 January 2007
#
# Copyright 2007, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
import configparser
import os

parent = configparser.ConfigParser


class ConfigManager(parent):
    """
    This class provides a single entrypoint for all configurable properties,
    namely the self.Prop dictionary.
    """

    __instance = None
    __prop = None

    def __init__(self):
        """
        Constructor.
        """
        configparser.ConfigParser.__init__(self)
        self.__prop = {}
        self._setProps()
        # Now look for an ac.ini file within
        # first the current directory and then
        # the users $HOME directory.  If not found
        # then the default values from setProps are used.
        config_file_name = "ac.ini"
        files = []
        # Append other paths to look in here.

        # Look in next one, the home directory of the user.
        files.append(os.path.join(os.environ["HOME"], config_file_name))
        # Look in last directory first, the current directory
        files.append(os.path.join(os.path.realpath(os.curdir), config_file_name))
        self.read(files)

    def getInstance():
        """
        Return instance of singleton.
        """
        if ConfigManager.__instance is None:
            ConfigManager.__instance = ConfigManager()
        return ConfigManager.__instance

    # define static method
    getInstance = staticmethod(getInstance)

    def _setProps(self):
        """
        Used only by constructor to set all ConfigParser defaults. Establishes
        a dictionary of sections and then a dictionary of keyword, value
        association for each section.
        """
        ################################################################
        # General parameters here. This is the DEFAULT section.
        ################################################################
        # self.__prop['DEFAULT'] = {}

        #
        # 'DEFAULT' section is baked in for Python 2.6x
        # the call below will raise ValueError
        #

        # This sets the defaults within a section.
        # self._setSectionDefaults('DEFAULT')
        ################################################################
        # schema parameters here.
        ################################################################
        # When using these, make sure to either prepend the build root env var or the appropriate amount of "../"
        self.__prop["schema"] = {}
        self.__prop["schema"][
            "commands"
        ] = "/Autocoders/Python/schema/default/command_schema.rng"
        self.__prop["schema"][
            "events"
        ] = "/Autocoders/Python/schema/default/event_schema.rng"
        self.__prop["schema"][
            "telemetry"
        ] = "/Autocoders/Python/schema/default/channel_schema.rng"
        self.__prop["schema"][
            "component"
        ] = "/Autocoders/Python/schema/default/component_schema.rng"
        self.__prop["schema"][
            "assembly"
        ] = "/Autocoders/Python/schema/default/topology_schema.rng"
        self.__prop["schema"][
            "deployment"
        ] = "/Autocoders/Python/schema/default/topology_schema.rng"
        self.__prop["schema"][
            "internal_interfaces"
        ] = "/Autocoders/Python/schema/default/internal_interface_schema.rng"
        self.__prop["schema"][
            "interface"
        ] = "/Autocoders/Python/schema/default/interface_schema.rng"
        self.__prop["schema"][
            "serializable"
        ] = "/Autocoders/Python/schema/default/serializable_schema.rng"
        self.__prop["schema"][
            "parameters"
        ] = "/Autocoders/Python/schema/default/parameters_schema.rng"
        self.__prop["schema"][
            "enum"
        ] = "/Autocoders/Python/schema/default/enum_schema.rng"
        self.__prop["schema"][
            "array"
        ] = "/Autocoders/Python/schema/default/array_schema.rng"
        self._setSectionDefaults("schema")
        ################################################################
        # Schematron parameters here.
        ################################################################
        self.__prop["schematron"] = {}
        self.__prop["schematron"][
            "chan_id"
        ] = "/Autocoders/Python/schema/default/channel_id_schematron.rng"
        self.__prop["schematron"][
            "param_id"
        ] = "/Autocoders/Python/schema/default/parameter_id_schematron.rng"
        self.__prop["schematron"][
            "cmd_op"
        ] = "/Autocoders/Python/schema/default/command_op_schematron.rng"
        self.__prop["schematron"][
            "evr_id"
        ] = "/Autocoders/Python/schema/default/event_id_schematron.rng"
        self.__prop["schematron"][
            "comp_unique"
        ] = "/Autocoders/Python/schema/default/comp_uniqueness_schematron.rng"
        self.__prop["schematron"][
            "top_unique"
        ] = "/Autocoders/Python/schema/default/top_uniqueness_schematron.rng"
        self.__prop["schematron"][
            "active_comp"
        ] = "/Autocoders/Python/schema/default/active_comp_schematron.rng"
        self.__prop["schematron"][
            "enum_value"
        ] = "/Autocoders/Python/schema/default/enum_value_schematron.rng"
        self.__prop["schematron"][
            "array_default"
        ] = "/Autocoders/Python/schema/default/array_schematron.rng"
        self._setSectionDefaults("schematron")
        ################################################################
        # component parameters here.
        ################################################################
        self.__prop["component"] = {}
        #
        # Default file naming corresponds to XML name and namespace
        # attributes if this is "True".  Otherwise it corresponds to
        # the input XML file name prefix if this is "False".
        #
        self.__prop["component"]["XMLDefaultFileName"] = "False"
        #
        # Define default file suffix here...
        #
        self.__prop["component"]["ComponentXML"] = "ComponentAi.xml"
        self.__prop["component"]["ComponentCpp"] = "ComponentAc.cpp"
        self.__prop["component"]["ComponentH"] = "ComponentAc.hpp"
        self.__prop["component"]["ImplCpp"] = ".cpp-template"
        self.__prop["component"]["ImplH"] = ".hpp-template"

        self.__prop["component"]["ComponentTestCpp"] = "ComponentTestAc.cpp"
        self.__prop["component"]["ComponentTestH"] = "ComponentTestAc.hpp"

        self.__prop["component"]["GTestCpp"] = "GTestAc.cpp"
        self.__prop["component"]["GTestH"] = "GTestAc.hpp"

        self.__prop["component"]["TestImplCpp"] = "TestImpl.cpp"
        self.__prop["component"]["TestImplH"] = "TestImpl.hpp"

        # This sets the defaults within a section.
        self._setSectionDefaults("component")
        ################################################################
        # port or interface parameters here.
        ################################################################
        self.__prop["port"] = {}
        #
        # Default file naming corresponds to XML name and namespace
        # attributes if this is "True".  Otherwise it corresponds to
        # the input XML file name prefix if this is "False".
        #
        self.__prop["port"]["XMLDefaultFileName"] = "False"
        #
        # Define default file suffix here...
        #
        self.__prop["port"]["PortXML"] = "PortAi.xml"
        self.__prop["port"]["PortCpp"] = "PortAc.cpp"
        self.__prop["port"]["PortH"] = "PortAc.hpp"
        #
        # This sets the defaults within a section.
        self._setSectionDefaults("port")
        ################################################################
        # assembly (topology) parameters here.
        ################################################################
        self.__prop["assembly"] = {}
        self.__prop["assembly"]["TopologyXML"] = "AppAi.xml"
        self.__prop["assembly"]["TopologyCpp"] = "AppAc.cpp"
        self.__prop["assembly"]["TopologyH"] = "AppAc.hpp"
        self.__prop["assembly"]["TopologyID"] = "AppID.csv"
        # defaults for start baseId and window size set here
        self.__prop["assembly"]["baseID"] = 0
        self.__prop["assembly"]["window"] = 20
        #
        self._setSectionDefaults("assembly")
        ################################################################
        # deployment (topology) parameters here.
        ################################################################
        self.__prop["deployment"] = {}
        self.__prop["deployment"]["TopologyXML"] = "AppAi.xml"
        self.__prop["deployment"]["TopologyCpp"] = "AppAc.cpp"
        self.__prop["deployment"]["TopologyH"] = "AppAc.hpp"
        # defaults for start baseId and window size set here
        self.__prop["deployment"]["baseID"] = 0
        self.__prop["deployment"]["window"] = 20
        #
        self._setSectionDefaults("deployment")
        ################################################################
        # This sets the defaults for generated include options
        # The main idea is playing with include header files for testing
        ################################################################
        self.__prop["includes"] = {}
        self.__prop["includes"]["comp_include_path"] = None
        self.__prop["includes"]["port_include_path"] = None
        self.__prop["includes"]["serial_include_path"] = None
        #
        self._setSectionDefaults("includes")
        ################################################################
        # Serializable parameters here.
        ################################################################
        self.__prop["serialize"] = {}
        #
        # Default file naming corresponds to XML name and namespace
        # attributes if this is "True".  Otherwise it corresponds to
        # the input XML file name prefix if this is "False".
        #
        self.__prop["serialize"]["XMLDefaultFileName"] = "False"
        #
        # Define default file suffix here...
        #
        self.__prop["serialize"]["SerializableXML"] = "SerializableAi.xml"
        self.__prop["serialize"]["SerializableCpp"] = "SerializableAc.cpp"
        self.__prop["serialize"]["SerializableH"] = "SerializableAc.hpp"
        self._setSectionDefaults("serialize")
        #
        ###############################################################
        # Special Ports and their attributes
        # They name's default to the implicit ports
        ###############################################################
        self.__prop["special_ports"] = {}

        self.__prop["special_ports"]["Cmd"] = {}
        self.__prop["special_ports"]["Cmd"]["role"] = "Cmd"
        self.__prop["special_ports"]["Cmd"]["name"] = "CmdDisp"
        self.__prop["special_ports"]["Cmd"]["type"] = "Fw::Cmd"
        self.__prop["special_ports"]["Cmd"]["direction"] = "Input"
        self.__prop["special_ports"]["Cmd"]["comment"] = None
        self.__prop["special_ports"]["Cmd"]["port_file"] = "Fw/Cmd/CmdPortAi.xml"

        self.__prop["special_ports"]["CmdResponse"] = {}
        self.__prop["special_ports"]["CmdResponse"]["name"] = "CmdStatus"
        self.__prop["special_ports"]["CmdResponse"]["type"] = "Fw::CmdResponse"
        self.__prop["special_ports"]["CmdResponse"]["direction"] = "Output"
        self.__prop["special_ports"]["CmdResponse"]["comment"] = None
        self.__prop["special_ports"]["CmdResponse"][
            "port_file"
        ] = "Fw/Cmd/CmdResponsePortAi.xml"

        self.__prop["special_ports"]["CmdRegistration"] = {}
        self.__prop["special_ports"]["CmdRegistration"]["name"] = "CmdReg"
        self.__prop["special_ports"]["CmdRegistration"]["type"] = "Fw::CmdReg"
        self.__prop["special_ports"]["CmdRegistration"]["direction"] = "Output"
        self.__prop["special_ports"]["CmdRegistration"]["comment"] = None
        self.__prop["special_ports"]["CmdRegistration"][
            "port_file"
        ] = "Fw/Cmd/CmdRegPortAi.xml"

        self.__prop["special_ports"]["LogEvent"] = {}
        self.__prop["special_ports"]["LogEvent"]["name"] = "Log"
        self.__prop["special_ports"]["LogEvent"]["type"] = "Fw::Log"
        self.__prop["special_ports"]["LogEvent"]["direction"] = "Output"
        self.__prop["special_ports"]["LogEvent"]["comment"] = None
        self.__prop["special_ports"]["LogEvent"]["port_file"] = "Fw/Log/LogPortAi.xml"

        self.__prop["special_ports"]["LogTextEvent"] = {}
        self.__prop["special_ports"]["LogTextEvent"]["name"] = "LogText"
        self.__prop["special_ports"]["LogTextEvent"]["type"] = "Fw::LogText"
        self.__prop["special_ports"]["LogTextEvent"]["direction"] = "Output"
        self.__prop["special_ports"]["LogTextEvent"]["comment"] = None
        self.__prop["special_ports"]["LogTextEvent"][
            "port_file"
        ] = "Fw/Log/LogTextPortAi.xml"

        self.__prop["special_ports"]["TimeGet"] = {}
        self.__prop["special_ports"]["TimeGet"]["name"] = "Time"
        self.__prop["special_ports"]["TimeGet"]["type"] = "Fw::Time"
        self.__prop["special_ports"]["TimeGet"]["direction"] = "Output"
        self.__prop["special_ports"]["TimeGet"]["comment"] = None
        self.__prop["special_ports"]["TimeGet"]["port_file"] = "Fw/Time/TimePortAi.xml"

        self.__prop["special_ports"]["Telemetry"] = {}
        self.__prop["special_ports"]["Telemetry"]["name"] = "Tlm"
        self.__prop["special_ports"]["Telemetry"]["type"] = "Fw::Tlm"
        self.__prop["special_ports"]["Telemetry"]["direction"] = "Output"
        self.__prop["special_ports"]["Telemetry"]["comment"] = None
        self.__prop["special_ports"]["Telemetry"]["port_file"] = "Fw/Tlm/TlmPortAi.xml"

        self.__prop["special_ports"]["ParamGet"] = {}
        self.__prop["special_ports"]["ParamGet"]["name"] = "ParamGet"
        self.__prop["special_ports"]["ParamGet"]["type"] = "Fw::PrmGet"
        self.__prop["special_ports"]["ParamGet"]["direction"] = "Output"
        self.__prop["special_ports"]["ParamGet"]["comment"] = None
        self.__prop["special_ports"]["ParamGet"][
            "port_file"
        ] = "Fw/Prm/PrmGetPortAi.xml"

        self.__prop["special_ports"]["ParamSet"] = {}
        self.__prop["special_ports"]["ParamSet"]["name"] = "ParamSet"
        self.__prop["special_ports"]["ParamSet"]["type"] = "Fw::PrmSet"
        self.__prop["special_ports"]["ParamSet"]["direction"] = "Output"
        self.__prop["special_ports"]["ParamSet"]["comment"] = None
        self.__prop["special_ports"]["ParamSet"][
            "port_file"
        ] = "Fw/Prm/PrmSetPortAi.xml"

        self._setSectionDefaults("special_ports")

        # Add constants processing

        self.__prop["constants"] = {}
        # where the constants file is relative to BUILD_ROOT
        # constants will be in the INI file format supported by the ConfigParser library
        self.__prop["constants"]["constants_file"] = os.environ.get(
            "FPRIME_AC_CONSTANTS_FILE", os.path.join("config", "AcConstants.ini")
        )
        self._setSectionDefaults("constants")

    def _setSectionDefaults(self, section):
        """
        For a section set up the default values.
        """
        self.add_section(section)
        for (key, value) in list(self.__prop[section].items()):
            self.set(section, key, "%s" % value)


if __name__ == "__main__":
    #
    # Quick test of configure defaults.
    #
    config = ConfigManager().getInstance()
    print()
    print("IPC section defaults:")
    for (key, value) in config.items("ipc"):
        print("{} = {}".format(key, value))
    print()
    print("Get some of the ipc values:")
    print("h_pub_suffix = %s" % config.get("ipc", "h_pub_suffix"))
    print("h_msg_suffix = %s" % config.get("ipc", "h_msg_suffix"))
    print("c_int_suffix = %s" % config.get("ipc", "c_int_suffix"))
    print("c_dispatch_suffix = %s" % config.get("ipc", "c_dispatch_suffix"))
    print("c_cmd_dispatch_suffix = %s" % config.get("ipc", "c_cmd_dispatch_suffix"))
