"""
@brief Loader class for importing xml based event dictionaries

@date Created July 19, 2018
@author R. Joseph Paetz

@bug No known bugs
"""

from fprime_gds.common.data_types import exceptions
from fprime_gds.common.templates.event_template import EventTemplate
from fprime_gds.common.utils.event_severity import EventSeverity

# Custom Python Modules
from .xml_loader import XmlLoader


class EventXmlLoader(XmlLoader):
    """Class to load xml based event dictionaries"""

    EVENT_SECT = "events"

    COMP_TAG = "component"
    NAME_TAG = "name"
    ID_TAG = "id"
    SEVERITY_TAG = "severity"
    FMT_STR_TAG = "format_string"
    DESC_TAG = "description"

    def construct_dicts(self, path):
        """
        Constructs and returns python dictionaries keyed on id and name

        This function should not be called directly, instead, use
        get_id_dict(path) and get_name_dict(path)

        Args:
            path: Path to the xml dictionary file containing event information

        Returns:
            A tuple with two event dictionaries (python type dict):
            (id_idct, name_dict). The keys are the events' id and name fields
            respectively and the values are ChTemplate objects
        """
        xml_tree = self.get_xml_tree(path)

        # Check if xml dict has events section
        event_section = self.get_xml_section(self.EVENT_SECT, xml_tree)
        if event_section is None:
            raise exceptions.GseControllerParsingException(
                "Xml dict did not have a %s section" % self.EVENT_SECT
            )

        id_dict = dict()
        name_dict = dict()

        for event in event_section:
            event_dict = event.attrib

            event_comp = event_dict[self.COMP_TAG]
            event_name = event_dict[self.NAME_TAG]
            event_id = int(event_dict[self.ID_TAG], base=16)
            event_severity = EventSeverity[event_dict[self.SEVERITY_TAG]]
            event_fmt_str = event_dict[self.FMT_STR_TAG]

            event_desc = None
            if self.DESC_TAG in event_dict:
                event_desc = event_dict[self.DESC_TAG]

            # Parse arguments
            args = self.get_args_list(event, xml_tree)

            event_temp = EventTemplate(
                event_id,
                event_name,
                event_comp,
                args,
                event_severity,
                event_fmt_str,
                event_desc,
            )

            id_dict[event_id] = event_temp
            name_dict[event_name] = event_temp

        return id_dict, name_dict
