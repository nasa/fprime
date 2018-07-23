'''
@brief Loader class for importing xml based event dictionaries

@date Created July 19, 2018
@author R. Joseph Paetz

@bug No known bugs
'''

# Custom Python Modules
from xml_loader import XmlLoader
from templates.event_template import EventTemplate
from controllers import exceptions


class EventXmlLoader(XmlLoader):
    '''Class to load xml based event dictionaries'''

    EVENT_SECT = "events"
    ARGS_SECT = "args"

    COMP_TAG = "component"
    NAME_TAG = "name"
    ID_TAG = "id"
    SEVERITY_TAG = "severity"
    FMT_STR_TAG = "format_string"
    ARG_NAME_TAG = "name"
    ARG_DESC_TAG = "description"
    ARG_TYPE_TAG = "type"

    def construct_dicts(self, path):
        '''
        Constructs and returns python dictionaries keyed on id and name

        This function should not be called directly, instead, use
        get_id_dict(path) and get_name_dict(path)

        Args:
            path: Path to the xml dictionary file containing event information

        Returns:
            A tuple with two event dictionaries (python type dict):
            (id_idct, name_dict). The keys are the events' id and name fields
            respectively and the values are ChTemplate objects
        '''
        xml_tree = self.get_xml_tree(path)

        # Check if xml dict has events section
        event_section = self.get_xml_section(self.EVENT_SECT, xml_tree)
        if (event_section == None):
            # TODO make this its own error (XML section err or something)
            raise exceptions.GseControllerParsingException(
                    "Xml dict did not have a %s section"%self.EVENT_SECT)

        id_dict = dict()
        name_dict = dict()

        for event in event_section:
            event_dict = ch.attrib

            event_comp = event_dict[self.COMP_TAG]
            event_name = event_dict[self.NAME_TAG]
            event_id = int(event_dict[self.ID_TAG], base=16)

