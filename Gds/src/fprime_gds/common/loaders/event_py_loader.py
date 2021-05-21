"""
@brief Loader class for importing python based event dictionaries

@date Created July 3, 2018
@author R. Joseph Paetz

@bug No known bugs
"""

from fprime.common.models.serialize.type_exceptions import TypeMismatchException
from fprime_gds.common.templates import event_template
from fprime_gds.common.utils.event_severity import EventSeverity

# Custom Python Modules
from . import python_loader


class EventPyLoader(python_loader.PythonLoader):
    """Class to load python based event dictionaries"""

    # Field names in the python module file dictionaries
    ID_FIELD = "ID"
    NAME_FIELD = "NAME"
    COMP_FIELD = "COMPONENT"
    SEVERITY_FIELD = "SEVERITY"
    FMT_STR_FIELD = "FORMAT_STRING"
    DESC_FIELD = "EVENT_DESCRIPTION"
    ARGS_FIELD = "ARGUMENTS"

    def construct_dicts(self, path):
        """
        Constructs and returns python dictionaries keyed on id and name

        This function should not be called directly, instead use
        get_id_dict(path) and get_name_dict(path)

        Args:
            path: Path to the python module file dictionary to convert. This
                  should be a directory. If using a regular fprime deployment,
                  this should be a path to the events dictionary in your
                  generated folder:
                  ${GENERATED_FOLDER_LOCATION}/generated/${DEPLOYMENT}/events

        Returns:
            A tuple with two event dictionaries (python type dict):
            (id_dict, name_dict). The keys should be the events' id and name
            fields respectively and the values should be EventTemplate
            objects.
        """
        module_dicts = self.read_dict(path, use_superpkg=True)

        id_dict = dict()
        name_dict = dict()

        for event_dict in module_dicts:

            try:
                # Create an event template object
                event_temp = event_template.EventTemplate(
                    event_dict[self.ID_FIELD],
                    event_dict[self.NAME_FIELD],
                    event_dict[self.COMP_FIELD],
                    event_dict[self.ARGS_FIELD],
                    EventSeverity[event_dict[self.SEVERITY_FIELD]],
                    event_dict[self.FMT_STR_FIELD],
                    event_dict[self.DESC_FIELD],
                )
                id_dict[event_dict[self.ID_FIELD]] = event_temp
                name_dict[event_dict[self.NAME_FIELD]] = event_temp
            except TypeMismatchException as error:
                print("Type mismatch: %s" % error.getMsg())
                raise error

        return id_dict, name_dict
