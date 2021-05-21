"""
chrono.py:

A chronologically-ordered history that relies on predicates to provide filtering, searching, and
retrieval operations. This history will re-order itself based on FSW time.

:author: koran
"""
from fprime.common.models.serialize.time_type import TimeType
from fprime_gds.common.history.history import History
from fprime_gds.common.testing_fw import predicates


class ChronologicalHistory(History):
    """
    A chronological history to support the GDS test api. This history adds support for specifying
    start with predicates and python's bracket notation.
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
        self.new_objects = []

        self.filter = predicates.always_true()
        if filter_pred is not None:
            if predicates.is_predicate(filter_pred):
                self.filter = filter_pred
            else:
                raise TypeError("The given filter was an instance of predicate.")

        self.retrieved_cursor = 0

    def data_callback(self, data, sender=None):
        """
        Data callback to push an object on the history. This callback will only add data_objects
        that satisfy the filter predicate.

        Args:
            data: object to store
            sender: unused API value
        """
        if self.filter(data):
            self.__insert_chrono(data, self.new_objects)
            index = self.__insert_chrono(data, self.objects)
            self.retrieved_cursor = min(index, self.retrieved_cursor)

    def retrieve(self, start=None):
        """
        Retrieve objects from this history. If a starting point is specified, will return a
        sub-list of all objects beginning at start to the latest object.
        Note: if no item satisfies the start predicate or the index is greater than the length of
        the history, an empty list will be returned.

        Args:
            start: optional first object to retrieve. can either be an index (int) or a predicate.
        Returns:
            a list of objects in chronological order
        """
        if start is None:
            index = 0
        else:
            index = self.__get_index(start, self.objects)
        self.retrieved_cursor = self.size()
        self.new_objects.clear()
        return self.objects[index:]

    def retrieve_new(self, repeats=False):
        """
        Retrieves a chronological order of objects that haven't been accessed through retrieve or
        retrieve_new before.

        Returns:
            a list of objects in chronological order
        """
        index = self.retrieved_cursor
        self.retrieved_cursor = self.size()

        if repeats:
            self.new_objects.clear()
            return self.objects[index:]
        else:
            new = self.new_objects
            self.new_objects = []
            return new

    def clear(self, start=None):
        """
        Clears objects from history. A clear that specifies a starting point will clear the history
        such that start becomes the earliest element in the history after objects are removed. If
        the start is specified as a predicate, start will be the earliest object to satisfy the
        predicate.
        Note: if no item satisfies the start predicate or the index is greater than the length of
        the history, all items will be cleared.

        Args:
            start: start: an optional indicator for the first item to remove. Can be a predicate, a
                TimeType or an index in the ordering
        """
        index = self.__clear_list(start, self.objects)

        if len(self.objects) > 0:
            start = self.objects[0].get_time()
            self.__clear_list(start, self.new_objects)
        else:
            self.new_objects.clear()

        self.retrieved_cursor -= index
        if self.retrieved_cursor < 0:
            self.retrieved_cursor = 0

    def size(self):
        """
        Accessor for the number of objects in the history
        Returns:
            the number of objects (int)
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

    ###########################################################################
    #   helper methods
    ###########################################################################
    @staticmethod
    def __insert_chrono(data_object, ordered):
        """
        traverses the existing order (back to front) and inserts the data object in the correct
        position chronologically.
        Args:
            data_object: an item to insert in the history. Must have a get_time() method.
            ordered: a list to insert the item into.
        Returns:
            the index that the item was inserted at (int)
        """
        for i, item in reversed(list(enumerate(ordered))):
            if item.get_time() < data_object.get_time():
                ordered.insert(i + 1, data_object)
                return i
        # If the data object is the earliest in the list or the list was empty
        ordered.insert(0, data_object)
        return 0

    def __clear_list(self, start, ordered):
        """
        finds the index that start specifies
        Args:
            start: an optional indicator for the first item to remove. Can be a predicate, a
                TimeType or an index in the ordering
            ordered: the list to clear
        Returns:
            the index in the given list that start refers to
        """
        if start is None:
            index = len(ordered)
        else:
            index = self.__get_index(start, ordered)
        del ordered[:index]
        return index

    @staticmethod
    def __get_index(start, ordered):
        """
        finds the index that start specifies
        Args:
            start: an indicator of a position in an order can be a predicate, a TimeType time
                stamp or an index in the ordering
            ordered: the list to clear
        Returns:
            the index in the given list that start refers to
        """
        if predicates.is_predicate(start):
            index = 0
            while index < len(ordered) and not start(ordered[index]):
                index += 1
            return index
        elif isinstance(start, TimeType):
            index = 0
            while index < len(ordered) and ordered[index].get_time() < start:
                index += 1
            return index
        else:
            return start
