"""
history.py:

An ordered history that defines what interfaces a history should have within the GDS

:author: koran
"""


class History:
    """
    An ordered history to support the GDS. Histories are intended to be registered with decoders in
    order to handle incoming objects and store them for retrieval. The default behavior of a
    history is to maintain objects in the order they were enqueued. However, should a sub-history
    want to maintain a different order, this should be made clear to the user and still support the
    calls in this History class.
    """

    def __init__(self):
        """
        Constructor used to set-up  a history.
        """
        raise NotImplementedError("This history didn't override the __init__ method.")

    def data_callback(self, data_object):
        """
        Data callback to push an object on the history. 

        Args:
            data_object: object to store
        """
        raise NotImplementedError("This history didn't override the data_callback method.")

    def retrieve(self, start=None):
        """
        Retrieve objects from this history. If a starting point is specified, will return a
        sub-list of all objects beginning at starting point in the order to the latest object.

        Args:
            start: a position in the history's order. Start should always be able to be specified
                by an index (int).
        Returns:
            an ordered list of objects
        """
        raise NotImplementedError("This history didn't override the retrieve method.")

    def retrieve_new(self):
        """
        Retrieves an ordered list of objects that have been enqueued since the last call to
        retrieve or retrieve_new.

        Returns:
            an ordered list of objects
        """
        raise NotImplementedError("This history didn't override the retrieve_new method.")

    def clear(self, start=None):
        """
        Clears objects from history. A clear that specifies a starting point will clear the history
        such that start becomes the earliest (with respect to the history's order) element in the
        history after objects are removed. 

        Args:
            start: a position in the history's order. Start should always be able to be specified
                by an index (int).
        """
        raise NotImplementedError("This history didn't override the clear method.")

    def size(self):
        """
        Accessor for the number of objects in the history
        Returns:
            the number of objects (int)
        """
        raise NotImplementedError("This history didn't override the size method.")
