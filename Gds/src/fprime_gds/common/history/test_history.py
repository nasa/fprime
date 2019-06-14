"""
History.py:

A chronological history that provides search features, clearing predicate callbacks 

:author: koran
"""

class TestHistory:
    """
    A chronological history to support the GDS test api. This is intended to be registered with the decoders 
    in order to handle incoming objects, and store them for retreival.
    """
    def __init__(self, log=None):
        """
        Constructor used to set-up history and, optionally, a log file
        :param log: If specified, the history will log objects to the file destination.
        """
        pass

    def data_callback(self, data_object):
        """
        Data callback to push an object on the history
        :param data_object: object to store
        """
        pass

    def retrieve(self, start=None):
        """
        Retrieve objects from this history
        :param start: return all objects newer than given start time
        :return: a list of objects
        """
        return self.objects

    def clear(self, index=None):
        """
        Clears items from history
        :param index: If specified, will only clear items before the given index. 
        """
        pass

    def find_earliest(self, search_pred):
        """
        Searches for the first instance of an object that satisfies the given predicate in a history.
        :param search_pred: A predicate to call on each object in the history. 
        :return: the first object to satisfy the given predicate.
        """
        pass

    def find_latest(self, search_pred):
        """
        Searches for the last instance of an object that satisfies the given predicate in a history.
        :param search_pred: A predicate that the search will use to choose an object. 
        :return: the last object to satisfy the given predicate.
        """
        pass
    
    def find_all(self, search_pred):
        """
        Searches for all instances in the history that satisfy the given predicate.
        :param search_pred: A predicate that the search will use to choose an object. 
        :return: all objects to satisfy the given predicate.
        """
        pass

    def count(self, search_pred):
        """
        Counts how many objects satisfy the given predicate.
        :param search_pred: 
        :return: the number of objects that satisfy the predicate

    