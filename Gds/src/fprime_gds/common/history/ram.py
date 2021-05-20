"""
ram.py:

A simple implementation of a history that maintains items in RAM. This is used for simplicity, but isn't exactly
robust nor persistent. Given that it is in the RAM, it is driven from the decoders object, which should run off the
middle-ware layer.

Note: this RAM history treats "start times" as session tokens to remember where it was last fetched from.

:author: lestarch
"""
import threading

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
        self.lock = threading.Lock()
        self.objects = []
        self.retrieved_cursors = {}

    def data_callback(self, data, sender=None):
        """
        Data callback to store

        :param data: object to store
        """
        with self.lock:
            self.objects.append(data)

    def retrieve(self, start=None):
        """
        Retrieve objects from this history. 'start' is the session token for retrieving new elements. If session is not
        specified, all elements are retrieved. If session is specified, then unseen elements are returned. If the
        session itself is new, it is recorded and set to the newest data.

        :param start: return all objects newer than given start session key
        :return: a list of objects
        """
        index = 0
        size = self.size()
        if start is not None:
            index = self.retrieved_cursors.get(start, size)
        with self.lock:
            objs = self.objects[index:size]
            self.retrieved_cursors[start] = size
        return objs

    def retrieve_new(self):
        """
        Retrieves a chronological order of objects that haven't been accessed through retrieve or
        retrieve_new before.

        Returns:
            a list of objects in chronological order
        """
        index = 0
        if len(self.retrieved_cursors.values()) > 0:
            index = max(self.retrieved_cursors.values())
        with self.lock:
            return self.objects[index:]

    def clear(self, start=None):
        """
        Clears objects from RamHistory. It clears upto the earliest session. If session is supplied, the session id will
        be deleted as well.

        Args:
            start: a position in the history's order (int).
        """
        with self.lock:
            try:
                if start is not None:
                    del self.retrieved_cursors[start]
            except KeyError:
                pass
            earliest = 0
            if len(self.retrieved_cursors.values()) > 0:
                earliest = min(self.retrieved_cursors.values())
            del self.objects[:earliest]
            for key in self.retrieved_cursors.keys():
                self.retrieved_cursors[key] -= earliest

    def size(self):
        """
        Accessor for the number of objects in the history
        Returns:
            the number of objects (int)
        """
        return len(self.objects)
