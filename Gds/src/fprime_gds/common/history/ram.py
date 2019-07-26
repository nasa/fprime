"""
ram.py:

A simple implementation of of a history that maintains items in RAM. This is used for simplicity, but isn't exactly
robust nor persistent. Given that it is in thr RAM, it is driven from the the decoders object, which should run off the
middle-ware layer.

:author: lestarch
"""
from fprime_gds.common.history.history import History


class RamHistory(History):
    """
    Chronological variant of history.  This is intended to be registered with the decoders in order
    to handle incoming objects, and store them for retrieval.
    """
    def __init__(self):
        """
        Constructor used to set-up in-memory store for history
        """
        self.objects = []
        self.retrieved_cursor = 0

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

    def retrieve_new(self):
        """
        Retrieves a chronological order of objects that haven't been accessed through retrieve or
        retrieve_new before.

        Returns:
            a list of objects in chronological order
        """
        index = self.retrieved_cursor
        self.retrieved_cursor = self.size()
        return self.objects[index:]

    def clear(self, start=None):
        """
        Clears objects from RamHistory. A clear that specifies a starting point will clear the
        history such that the element at the start index becomes the earliest element in the
        history after objects are removed.

        Args:
            start: a position in the history's order (int).
        """
        if start is None:
            index = self.size()
        else:
            index = start

        self.retrieved_cursor -= index
        if self.retrieved_cursor < 0:
            self.retrieved_cursor = 0

        del self.objects[:index]

    def size(self):
        """
        Accessor for the number of objects in the history
        Returns:
            the number of objects (int)
        """
        return len(self.objects)
