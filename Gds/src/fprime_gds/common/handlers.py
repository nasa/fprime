"""
handlers.py:

Defines a set of base classes that allow for the system to handle various functions of the system. Primarily this
defines the "DataHandler" base class for handling data.

@author mstarch
"""
import abc


class DataHandler(abc.ABC):
    """
    Defines the necessary functions required to handle data as part of the F prime project. This allows any implementer
    to be used to handle data.
    """

    @abc.abstractmethod
    def data_callback(self, data, sender=None):
        """
        Callback function used to handle data being produced elsewhere in the system and processed by the given object.
        Data supplied should be of a known type for the given object, and sender is an id of the sender. If not supplied
        sender will be None.

        :param data: data to be handled by this class
        :param sender: (optional) id of sender, otherwise None
        """


class HandlerRegistrar(abc.ABC):
    """
    Defines a class that will take in registrants and remember them for calling back later. These objects should be of
    the type "DataHandler" as this handler will send data back to these handlers when asked to do so.
    """

    def __init__(self):
        """
        Constructor defining the internal lists needed to store the registrants.
        """
        super().__init__()
        self._registrants = []

    def register(self, registrant):
        """
        Register a registrant with this registrar. Will be stored and called back when asked to send data to all the
        handlers registered.

        :param registrant: handler to register
        """
        if not isinstance(registrant, DataHandler):
            raise ValueError("Cannot register non data handler")
        self._registrants.append(registrant)

    def deregister(self, registrant):
        """
        Remove a registrant from the registrar such that it will not be called back later. Note: ignores invalid
        removals by trapping the error, as the desired effect is already satisfied.

        :param registrant: registrant to remove
        :return: True if found, False if not. May safely be ignored.
        """
        try:
            self._registrants.remove(registrant)
            return True
        except ValueError:
            return False

    def send_to_all(self, data, sender=None):
        """
        Sends the given data to all registrants.

        :param data: data to send back to registrants
        :param sender: (optional) sender to pass to data_callback
        """
        for registrant in self._registrants:
            registrant.data_callback(data, sender)
