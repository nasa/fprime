"""
@brief Loader class for importing xml based channel dictionaries

@date Created July 19, 2018
@author R. Joseph Paetz

@bug No known bugs
"""

from fprime_gds.common.data_types import exceptions
from fprime_gds.common.templates.ch_template import ChTemplate

# Custom Python Modules
from .xml_loader import XmlLoader


class ChXmlLoader(XmlLoader):
    """Class to load xml based telemetry channel dictionaries"""

    CH_SECT = "channels"  # Name of the channel section

    # Xml Tags for important fields
    COMP_TAG = "component"
    NAME_TAG = "name"
    ID_TAG = "id"
    DESC_TAG = "description"
    TYPE_TAG = "type"
    FMT_STR_TAG = "format_string"
    LOW_R_TAG = "low_red"
    LOW_O_TAG = "low_orange"
    LOW_Y_TAG = "low_yellow"
    HIGH_Y_TAG = "high_yellow"
    HIGH_O_TAG = "high_orange"
    HIGH_R_TAG = "high_red"

    def construct_dicts(self, path):
        """
        Constructs and returns python dictionaries keyed on id and name

        This function should not be called directly, instead, use
        get_id_dict(path) and get_name_dict(path)

        Args:
            path: Path to the xml dictionary file containing channel information

        Returns:
            A tuple with two channel dictionaries (python type dict):
            (id_idct, name_dict). The keys are the channels' id and name fields
            respectively and the values are ChTemplate objects
        """
        xml_tree = self.get_xml_tree(path)

        # Check if xml dict has channels section
        ch_section = self.get_xml_section(self.CH_SECT, xml_tree)
        if ch_section is None:
            raise exceptions.GseControllerParsingException(
                "Xml dict did not have a %s section" % self.CH_SECT
            )

        id_dict = dict()
        name_dict = dict()
        for ch in ch_section:
            ch_dict = ch.attrib

            # Assume the required fields are present (component, name, id,
            #  description, type). Check for all others
            ch_comp = ch_dict[self.COMP_TAG]
            ch_name = ch_dict[self.NAME_TAG]
            ch_id = int(ch_dict[self.ID_TAG], base=16)
            ch_type_obj = self.parse_type(ch_dict[self.TYPE_TAG], ch, xml_tree)

            ch_desc = None
            ch_fmt_str = None
            ch_low_red = None
            ch_low_orange = None
            ch_low_yellow = None
            ch_high_yellow = None
            ch_high_orange = None
            ch_high_red = None

            if self.DESC_TAG in ch_dict:
                ch_desc = ch_dict[self.DESC_TAG]

            if self.FMT_STR_TAG in ch_dict:
                ch_fmt_str = ch_dict[self.FMT_STR_TAG]

            # TODO we need to convert these into numbers, is this the best
            #  way to do it?
            if self.LOW_R_TAG in ch_dict:
                ch_low_red = float(ch_dict[self.LOW_R_TAG])

            if self.LOW_O_TAG in ch_dict:
                ch_low_orange = float(ch_dict[self.LOW_O_TAG])

            if self.LOW_Y_TAG in ch_dict:
                ch_low_yellow = float(ch_dict[self.LOW_Y_TAG])

            if self.HIGH_Y_TAG in ch_dict:
                ch_high_yellow = float(ch_dict[self.HIGH_Y_TAG])

            if self.HIGH_O_TAG in ch_dict:
                ch_high_orange = float(ch_dict[self.HIGH_O_TAG])

            if self.HIGH_R_TAG in ch_dict:
                ch_high_red = float(ch_dict[self.HIGH_R_TAG])

            ch_temp = ChTemplate(
                ch_id,
                ch_name,
                ch_comp,
                ch_type_obj,
                ch_fmt_str,
                ch_desc,
                ch_low_red,
                ch_low_orange,
                ch_low_yellow,
                ch_high_yellow,
                ch_high_orange,
                ch_high_red,
            )

            id_dict[ch_id] = ch_temp
            name_dict[ch_name] = ch_temp

        return id_dict, name_dict
