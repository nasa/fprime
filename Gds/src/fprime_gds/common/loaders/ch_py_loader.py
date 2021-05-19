"""
@brief Loader class for importing python based channel dictionaries

@date Created July 11, 2018
@author R. Joseph Paetz

@bug No known bugs
"""

from fprime_gds.common.templates.ch_template import ChTemplate

# Custom Python Modules
from .python_loader import PythonLoader


class ChPyLoader(PythonLoader):
    """Class to load python based telemetry channel dictionaries"""

    # Field names in the python module files (used to construct dictionaries)
    ID_FIELD = "ID"
    NAME_FIELD = "NAME"
    COMP_FIELD = "COMPONENT"
    DESC_FIELD = "CHANNEL_DESCRIPTION"
    TYPE_FIELD = "TYPE"
    FMT_STR_FIELD = "FORMAT_STRING"
    LOW_R_FIELD = "LOW_RED"
    LOW_O_FIELD = "LOW_ORANGE"
    LOW_Y_FIELD = "LOW_YELLOW"
    HIGH_Y_FIELD = "HIGH_YELLOW"
    HIGH_O_FIELD = "HIGH_ORANGE"
    HIGH_R_FIELD = "HIGH_RED"

    def construct_dicts(self, path):
        """
        Constructs and returns python dictionaries keyed on id and name

        This function should not be called directly, instead, use
        get_id_dict(path) and get_name_dict(path)

        Args:
            path: Path to the python module file dictionary to convert. This
                  should be a directory. If using a regular fprime deployment,
                  this should be a path to the events dictionary in your
                  generated folder:
                  ${GENERATED_FOLDER_LOCATION}/generated/${DEPLOYMENT}/channels

        Returns:
            A tuple with two channel dictionaries (python type dict):
            (id_dict, name_dict). The keys should be the channels' id and
            name fields respectively and the values should be ChTemplate
            objects.
        """
        # We do need it sometimes, so if we don't always set it to true, we will need to pass an arg
        module_dicts = self.read_dict(path, use_superpkg=True)

        id_dict = dict()
        name_dict = dict()

        for ch_dict in module_dicts:
            # Create a channel template object
            ch_temp = ChTemplate(
                ch_dict[self.ID_FIELD],
                ch_dict[self.NAME_FIELD],
                ch_dict[self.COMP_FIELD],
                ch_dict[self.TYPE_FIELD],
                ch_dict[self.FMT_STR_FIELD],
                ch_dict[self.DESC_FIELD],
                ch_dict[self.LOW_R_FIELD],
                ch_dict[self.LOW_O_FIELD],
                ch_dict[self.LOW_Y_FIELD],
                ch_dict[self.HIGH_Y_FIELD],
                ch_dict[self.HIGH_O_FIELD],
                ch_dict[self.HIGH_R_FIELD],
            )

            id_dict[ch_dict[self.ID_FIELD]] = ch_temp
            name_dict[ch_dict[self.NAME_FIELD]] = ch_temp

        return id_dict, name_dict
