"""
test_history.py:

A history that relies on predicates to provide filtering,
searching, and datastructure operations

:author: koran
"""
from utils import predicates


class TestHistory:
    """
    A chronological history to support the GDS test api. This is intended to be
    registered with the decoders in order to handle incoming objects, and store
    them for retrieval.
    """

    ###########################################################################
    #   History Functions
    ###########################################################################
    def __init__(self, filter_pred=None, log=None):
        """
        Constructor used to set-up history and, optionally, a log file
        :param filter_pred: If specified, the history will ignore (drop) objects that
            don't satisfy the filter predicate.
        :param log: If specified, the history will log objects to the file destination.
        """
        self.objects = []

        if predicates.is_predicate(filter_pred):
            self.filter = filter_pred
        else:
            # TODO raise error
            pass
        # TODO implement logging

    def data_callback(self, data_object):
        """
        Data callback to push an object on the history. This callback will only add
        data_objects that satisfy the filter predicate.
        :param data_object: object to store
        """
        if(self.filter(data_object)):
            self.objects.append(data_object)

    def retrieve(self, start_pred=None):
        """
        Retrieve objects from this history
        :param start_pred: If specified, will return a list of only the first object to
            satisfy this predicate and all following objects
        :return: a list of objects
        """
        index = 0
        if predicates.is_predicate(start_pred):
            while not start_pred(self.objects[index]):
                index += 1
        return self.objects[index:]

    def __get_item__(self, index):
        """
        __get_item__ is a special method in python that allows using brackets.
        Example: item = history[2] # this would return the second item in the history.

        :param index: the index of the array to return.
        :return: the item at the index specified.
        """
        return self.objects[index]

    def clear(self, start_pred=None):
        """
        Clears objects from history
        :param start_pred: If specified, will clear all objects before the first to
            satisfy the start_predicate
        """
        index = 0
        if predicates.is_predicate(start_pred):
            while not start_pred(self.objects[index]):
                index += 1
        # TODO remove up until index

    def size(self):
        """
        Accessor for the number of objects in the history
        :return: the number of objects
        """
        return self.__len__()

    def __len__(self):
        """
        Accessor for the number of objects in the history
        :return: the number of objects
        """
        return len(self.objects)

    ###########################################################################
    #   Search Functions
    ###########################################################################
    def find_earliest(self, search_pred):
        """
        Searches for the first instance of an object that satisfies the given predicate
        in a history
        :param search_pred: A predicate to call on each object in the history
        :return: the first object to satisfy the given predicate
        """
        pass

    def find_latest(self, search_pred):
        """
        Searches for the last instance of an object that satisfies the given predicate in
        a history
        :param search_pred: A predicate that the search will use to choose an object
        :return: the last object to satisfy the given predicate
        """
        pass

    def find_all(self, search_pred):
        """
        Searches for all instances in the history that satisfy the given predicate
        :param search_pred: A predicate that the search will use to choose an object
        :return: an array of all objects to satisfy the given predicate
        """
        pass

    def count(self, search_pred, start_pred=None):
        """
        Counts how many objects satisfy the given predicate
        :param search_pred: A predicate that the search will use to choose an object
        :return: the number of objects that satisfy the predicate
        """
        pass
