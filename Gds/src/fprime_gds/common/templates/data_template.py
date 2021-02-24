"""
@brief Base data template class.

Data templates are classes whose instances describe a specific class of data
items. For example, a data template instance could describe the AF_ASSERT_0
event or the channel FR_CycleTime.

@date Created July 2, 2018
@author R. Joseph Paetz

@bug No known bugs
"""
import fprime_gds.common.utils.jsonable


class DataTemplate:
    """Base class for all data template classes"""

    def __init__(self):
        """
        Constructor.

        The only required fields for template classes are id and name

        Returns:
            Initialized Template object
        """
        # Initialize at least id and name here

    def get_id(self):
        raise NotImplementedError

    def get_name(self):
        raise NotImplementedError

    def to_jsonable(self):
        """
        Converts to a JSONable object (primitives, anon-objects, lists)
        """
        return fprime_gds.common.utils.jsonable.fprime_to_jsonable(self)
