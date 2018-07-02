'''
@brief Base data template class.

Data templates are classes whose instances describe a specific class of data
items. For example, a data template instance could describe the AF_ASSERT_0
event or the channel FR_CycleTime.

@date Created July 2, 2018
@author R. Joseph Paetz

@bug No known bugs
'''


class DataTemplate(object):
    '''Base class for all data template classes'''

    def __init__(self):
        '''
        Constructor.

        The only required fields are id and name

        Returns:
            Initialized Template object
        '''
        # Check if the field is already initialized in case this constructor
        # is getting called from a subclass constructor
        if not self.id:
            self.id = 0
        if not self.name:
            self.name = ""


    def get_id(self):
        return self.id

    def get_name(self):
        return self.name
