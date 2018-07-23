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
from utils.event_severity import EventSeverity

class EventXmlLoader(XmlLoader):
    '''Class to load xml based event dictionaries'''

    EVENT_SECT = "events"
    ARGS_SECT = "args"

    COMP_TAG = "component"
    NAME_TAG = "name"
    ID_TAG = "id"
    SEVERITY_TAG = "severity"
    FMT_STR_TAG = "format_string"
    DESC_TAG = "description"
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
            event_dict = event.attrib

            event_comp = event_dict[self.COMP_TAG]
            event_name = event_dict[self.NAME_TAG]
            event_id = int(event_dict[self.ID_TAG], base=16)
            event_severity = EventSeverity[event_dict[self.SEVERITY_TAG]]
            event_fmt_str = event_dict[self.FMT_STR_TAG]

            event_desc = None
            if (self.DESC_TAG in event_dict):
                event_desc = event_dict[self.DESC_TAG]

            # Parse arguments
            args = []
            args_section = self.get_xml_section(self.ARGS_SECT, event)

            if args_section != None:
                for arg in args_section:
                    arg_dict = arg.attrib

                    arg_name = arg_dict[self.ARG_NAME_TAG]
                    arg_type_name = arg_dict[self.ARG_TYPE_TAG]
                    arg_typ_obj = self.parse_type(arg_type_name, arg, xml_tree)

                    arg_desc = None
                    if (self.ARG_DESC_TAG in arg_dict):
                        arg_desc = arg_dict[self.ARG_DESC_TAG]

                    args.append((arg_name, arg_desc, arg_typ_obj))

            event_temp = EventTemplate(event_id, event_name, event_comp,
                                       args, event_severity, event_fmt_str,
                                       event_desc)

            id_dict[event_id] = event_temp
            name_dict[event_name] = event_temp

        return (id_dict, name_dict)

