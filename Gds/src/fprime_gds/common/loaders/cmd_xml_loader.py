"""
@brief Loader class for importing xml based command dictionaries

@date Created July 23, 2018
@author R. Joseph Paetz

@bug No known bugs
"""

from fprime_gds.common.data_types import exceptions
from fprime_gds.common.templates.cmd_template import CmdTemplate

# Custom Python Modules
from .xml_loader import XmlLoader


class CmdXmlLoader(XmlLoader):
    """Class to load xml based command dictionaries"""

    CMD_SECT = "commands"
    EVENT_SECT = "events"

    COMP_TAG = "component"
    MNEMONIC_TAG = "mnemonic"
    OPCODE_TAG = "opcode"
    DESC_TAG = "description"

    def construct_dicts(self, path):
        """
        Constructs and returns python dictionaries keyed on id and name

        This function should not be called directly, instead, use
        get_id_dict(path) and get_name_dict(path)

        Args:
            path: Path to the xml dictionary file containing command information

        Returns:
            A tuple with two command dictionaries (python type dict):
            (id_dict, name_dict). The keys are the events' id and name fields
            respectively and the values are CmdTemplate objects
        """
        xml_tree = self.get_xml_tree(path)

        # Check if xml dict has commands section
        cmd_section = self.get_xml_section(self.CMD_SECT, xml_tree)
        if cmd_section is None:
            raise exceptions.GseControllerParsingException(
                "Xml dict did not have a %s section" % self.EVENT_SECT
            )

        id_dict = dict()
        name_dict = dict()

        for cmd in cmd_section:
            cmd_dict = cmd.attrib

            cmd_comp = cmd_dict[self.COMP_TAG]
            cmd_mnemonic = cmd_dict[self.MNEMONIC_TAG]
            cmd_opcode = int(cmd_dict[self.OPCODE_TAG], base=16)

            cmd_desc = None
            if self.DESC_TAG in cmd_dict:
                cmd_desc = cmd_dict[self.DESC_TAG]

            # Parse Arguments
            args = self.get_args_list(cmd, xml_tree)

            cmd_temp = CmdTemplate(cmd_opcode, cmd_mnemonic, cmd_comp, args, cmd_desc)

            id_dict[cmd_opcode] = cmd_temp
            name_dict["{}.{}".format(cmd_comp, cmd_mnemonic)] = cmd_temp

        return id_dict, name_dict
