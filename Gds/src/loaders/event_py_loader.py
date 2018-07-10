'''
@brief Loader class for importing python based event dictionaries

@date Created July 3, 2018
@author R. Joseph Paetz

@bug No known bugs
'''

# Custom Python Modules
import python_loader
from templates import event_template

class EventPyLoader(python_loader.PythonLoader):
    '''Class to load python based event dictionaries'''

    # Field names in the python module file dictionaries
    ID_FIELD =       "ID"
    NAME_FIELD =     "NAME"
    SEVERITY_FIELD = "SEVERITY"
    FMT_STR_FIELD =  "FORMAT_STRING"
    DESC_FIELD =     "EVENT_DESCRIPTION"
    ARGS_FIELD =     "ARGUMENTS"


    def __init__(self):
        '''
        Constructor

        Returns:
            An initialized loader object
        '''
        super(EventPyLoader, self).__init__()



    # TODO does this strange interface where you actually only call functions
    # defined in the python loader and not this very convinient looking function
    # make things more confusing than they need to be?
    def construct_dict(self, path):
        '''
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
            (id_dict, name_dict). They should have keys of the id and name
            fields respectively and the values for both should be event_template
            objects.
        '''
        # TODO currently we are always using the superpkg, is that OK?
        module_dicts = self.read_dict(path, use_superpkg=True)

        id_dict = dict()
        name_dict = dict()

        for event_dict in module_dicts:
            # Create an event template object
            event_temp = event_template.EventTemplate(event_dict[self.ID_FIELD],
                                                      event_dict[self.NAME_FIELD],
                                                      event_dict[self.SEVERITY_FIELD],
                                                      event_dict[self.FMT_STR_FIELD],
                                                      event_dict[self.DESC_FIELD],
                                                      event_dict[self.ARGS_FIELD])

            id_dict[event_dict[self.ID_FIELD]] = event_temp
            name_dict[event_dict[self.NAME_FIELD]] = event_temp

        return (id_dict, name_dict)


