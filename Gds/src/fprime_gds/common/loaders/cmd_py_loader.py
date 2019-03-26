'''
@brief Loader class for importing python based command dictionaries

@date Created July 9, 2018
@author R. Joseph Paetz

@bug No known bugs
'''
from __future__ import absolute_import

# Custom Python Modules
from . import python_loader
from fprime_gds.common.templates import cmd_template

class CmdPyLoader(python_loader.PythonLoader):
    '''Class to load python based command dictionaries'''

    # Field names in the python module file dictionaries
    COMPONENT_FIELD =   "COMPONENT"
    MNEMONIC_FIELD =    "MNEMONIC"
    OP_CODE_FIELD =     "OP_CODE"
    DESC_FIELD =        "CMD_DESCRIPTION"
    ARGS_FIELD =        "ARGUMENTS"


    def __init__(self):
        '''
        Constructor

        Returns:
            An initialized loader object
        '''
        super(CmdPyLoader, self).__init__()


    def construct_dicts(self, path):
        '''
        Constructs and returns python dictionaries keyed on id and name

        This function should not be called directly, instead use
        get_id_dict(path) and get_name_dict(path) defined in the base Loader
        class. For the command dictionary, the op_code is treated as the ID and
        the command mnemonic is treated as the name.

        Args:
            path: Path to the python module file dictionary to convert. This
                  should be a directory. If using a regular fprime deployment,
                  this should be a path to the events dictionary in your
                  generated folder:
                  ${GENERATED_FOLDER_LOCATION}/generated/${DEPLOYMENT}/events

        TODO: this description doesn't make sense
        Returns:
            A tuple with two event dictionaries (python type dict):
            (id_dict, name_dict). They should have keys of the id and name
            fields respectively and the values for both should be event_template
            objects.
        '''
        # TODO we are always using the superpkg, is that OK?
        module_dicts = self.read_dict(path, use_superpkg=True)

        id_dict = dict()
        name_dict = dict()

        for cmd_dict in module_dicts:
            # Create a cmd template object
            cmd_temp = cmd_template.CmdTemplate(cmd_dict[self.OP_CODE_FIELD],
                                                cmd_dict[self.MNEMONIC_FIELD],
                                                cmd_dict[self.COMPONENT_FIELD],
                                                cmd_dict[self.ARGS_FIELD],
                                                cmd_dict[self.DESC_FIELD])

            id_dict[cmd_dict[self.OP_CODE_FIELD]] = cmd_temp
            name_dict[cmd_dict[self.MNEMONIC_FIELD]] = cmd_temp

        return (id_dict, name_dict)

