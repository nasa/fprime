"""
ram.py:

A simple implementation of of a history that maintains items in RAM. This is used for simplicity, but isn't exactly
robust nor persistent. Given that it is in thr RAM, it is driven from the the decoders object, which should run off the
middle-ware layer.

:author: lestarch
"""


class RamHistory:
    """
    Chronological variant of history.  This is intended to be registered with the decoders in order to handle incoming
    objects, and store them for retreival.
    """
    def __init__(self):
        """
        Constructor used to set-up in-memory store for history
        """
        self.objects = []

    def data_callback(self, data_object):
        """
        Data callback to store
        :param data_object: object to store
        """
        self.objects.append(data_object)

    def retrieve(self, start=None):
        """
        Retrieve objects from this history
        :param start: return all objects newer than given start time
        :return: a list of objects
        """
        return self.objects
