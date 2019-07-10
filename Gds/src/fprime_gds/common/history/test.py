"""
test_history.py:

A chronological history that relies on predicates to provide filtering, searching, and data
structure operations

:author: koran
"""
from fprime_gds.common.testing_fw import predicates


class TestHistory:
    """
    A chronological history to support the GDS test api. This is intended to be registered with the
    decoders in order to handle incoming objects, and store them for retrieval.
    """

    ###########################################################################
    #   History Functions
    ###########################################################################
    def __init__(self, filter_pred=None):
        """
        Constructor used to set-up history. If the history is given a filter, it will ignore (drop)
        objects that don't satisfy the filter predicate.

        Args:
            filter_pred: an optional predicate to filter incoming data_objects
        """
        self.objects = []

        self.filter = predicates.always_true()
        if predicates.is_predicate(filter_pred):
            self.filter = filter_pred
        else:
            # TODO raise error
            pass

        self.retrieved_cursor = 0

    def data_callback(self, data_object):
        """
        Data callback to push an object on the history. This callback will only add data_objects
        that satisfy the filter predicate.

        Args:
            data_object: object to store
        """
        if self.filter(data_object):
            self.objects.append(data_object)

    def retrieve(self, start=None):
        """
        Retrieve objects from this history. If a starting point is specified, will return a
        sub-list of all objects beginning at start to the latest object.
        Note: if no item satisfies the start predicate or the index is greater than the length of
        the history, an empty list will be returned.

        Args:
            start: first object to retrieve. can either be an index or a predicate.
        Returns:
            a list of objects in chronological order
        """
        if start is None:
            index = 0
        elif predicates.is_predicate(start):
            index = 0
            while index < self.size() and not start(self.objects[index]):
                index += 1
        else:
            index = start

        self.retrieved_cursor = self.size()

        return self.objects[index:]

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
        Clears objects from history. A clear that specifies a starting point will clear the history
        such that start becomes the earliest element in the history after objects are removed. If
        the start is specified as a predicate, start will be the earliest object to satisfy the
        predicate.
        Note: if no item satisfies the start predicate or the index is greater than the length of
        the history, all items will be cleared.

        Args:
            start: clear all objects before start. start can either be an index or a predicate.
        """
        if start is None:
            index = self.size()
        elif predicates.is_predicate(start):
            index = 0
            while index < self.size() and not start(self.objects[index]):
                index += 1
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
            the number of objects
        """
        return len(self.objects)

    ###########################################################################
    #   Python Special Methods
    ###########################################################################
    def __len__(self):
        """
        Accessor for the number of objects in the history
        Returns:
            the number of objects
        """
        return self.size()

    def __getitem__(self, index):
        """
        __get_item__ is a special method in python that allows using brackets.
        Example: item = history[2] # this would return the second item in the history.

        Args:
            index: the index of the array to return.
        Returns:
            the item at the index specified.
        """
        return self.objects[index]
