'''
@brief Decoder for event data

This decoder takes in serialized events, parses them, and packages the results
in event_data objects.

Example data structure:
    +-------------------+---------------------+---------------------- - - -
    | ID (4 bytes)      | Time Tag (11 bytes) | Event argument data....
    +-------------------+---------------------+---------------------- - - -

@date Created June 29, 2018
@author R. Joseph Paetz

@bug No known bugs
'''
import copy

import decoder
from data_types import event_data
from models.serialize import u32_type
from models.serialize import time_type
from models.serialize.type_exceptions import *
import traceback

class EventDecoder(decoder.Decoder):
    '''Decoder class for event data'''

    def __init__(self, event_dict):
        '''
        EventDecoder class constructor

        Args:
            event_dict: Event dictionary. Event IDs should be keys and
                        EventTemplate objects should be values

        Returns:
            An initialized EventDecoder object.
        '''
        super(EventDecoder, self).__init__()

        self.__dict = event_dict


    def data_callback(self, data):
        '''
        Function called to pass data to the decoder class

        Args:
            data: Binary data to decode and pass to registered consumers
        '''
        self.send_to_all(self.decode_api(data))


    def decode_api(self, data):
        '''
        Decodes the given data and returns the result.

        This function allows for non-registered code to call the same decoding
        code as is used to parse data passed to the data_callback function.

        Args:
            data: Binary data to decode

        Returns:
            Parsed version of the event data in the form of a EventData object
            or None if the data is not decodable
        '''
        ptr = 0

        # Decode event ID here...
        id_obj = u32_type.U32Type()
        id_obj.deserialize(data, ptr)
        ptr += id_obj.getSize()
        event_id = id_obj.val
        #TODO remove
        #print("id=%d"%event_id)

        # Decode time...
        event_time = time_type.TimeType()
        event_time.deserialize(data, ptr)
        ptr += event_time.getSize()
        #print("time=%s"%event_time)

        if event_id in self.__dict:
            event_temp = self.__dict[event_id]

            arg_vals = self.decode_args(data, ptr, event_temp)

            return event_data.EventData(arg_vals, event_time, event_temp)
        else:
            print("Event decode error: id %d not in dictionary"%event_id)
            return None


    def decode_args(self, arg_data, offset, template):
        '''
        Decodes the serialized event arguments

        The event arguments are each serialized and then appended to each other.
        Parse that section of the data into the individual arguments.

        Args:
            arg_data: Serialized argument data to parse
            offset: Offset into the arg_data where parsing should start
            template: EventTemplate object that describes the type of event the
                      arg_data goes to.

        Returns:
            Parsed arguments in a tuple (order the same as they were parsed in).
            Each element in the tuple is an instance of the same class as the
            corresponding arg_type object in the template parameter. Returns
            none if the arguments can't be parsed
        '''
        arg_results = []
        args = template.get_args()

        # For each argument, use the arg_obj deserialize method to get the value
        for arg in args:
            (arg_name, arg_desc, template_arg_obj) = arg

            # Create a new instance of the argument's type object so we don't
            # use the template's object for deserialization and storage of the
            # parsed argument value.
            arg_obj = copy.deepcopy(template_arg_obj)

            try:
                arg_obj.deserialize(arg_data, offset)
                arg_results.append(arg_obj)
            except TypeException as e:
                print("Event decode exception %s"%(e.getMsg()))
                traceback.print_exc()
                return None


            offset = offset + arg_obj.getSize()

        return tuple(arg_results)


if __name__ == "__main__":
    pass

