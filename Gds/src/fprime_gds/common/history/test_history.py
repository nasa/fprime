"""
test_history.py:

A chronological history that relies on predicates to provide filtering, searching, and datastructure operations. 

:author: koran
"""

class TestHistory:
    """
    A chronological history to support the GDS test api. This is intended to be registered with the decoders 
    in order to handle incoming objects, and store them for retrieval.
    """
    ###################################################################################
    ##  History Functions
    ###################################################################################
    def __init__(self, filter_pred=None, log=None):
        """
        Constructor used to set-up history and, optionally, a log file
        :param filter_pred: If specified, the history will ignore (drop) objects that don't satisfy the filter predicate. 
        :param log: If specified, the history will log objects to the file destination.
        """
        pass

    def data_callback(self, data_object):
        """
        Data callback to push an object on the history
        :param data_object: object to store
        """
        pass

    def retrieve(self, start_pred=None):
        """
        Retrieve objects from this history
        :param start_pred: If specified, will return a list of only the first object to satisfy this predicate and all following objects
        :return: a list of objects
        """
        return self.objects

    def clear(self, start_pred=None):
        """
        Clears objects from history
        :param start_pred: If specified, will clear all objects before the first to satisfy the start_predicate
        """
        pass

    def size(self):
        """
        Accessor for the number of objects in the history
        :return: the number of objects
        """
        pass

    ###################################################################################
    ##  Subscriber Functions (TODO: to be moved to the standard pipeline)
    ###################################################################################
    def register_callback(self, filter_pred, callback):
        """
        Registers a callback that will be called and passed a copy of the most recent 
        data-object when ever this history receives an object that satisfies the filter 
        predicate.
        :param filter_pred: The predicate that will be used to narrow the history. This predicate is also used as a key to deregister this callback.
        :return: a boolean of whether or not the callback was successfully added.
        """
        pass

    def remove_callback(self, filter_pred):
        """
        Clears items from history
        :param filter_pred: The predicate that will be used to narrow the history. This predicate is also used as a key to deregister this callback.
        :return: a boolean of whether or not the callback was found and removed.
        """
        pass

    def remove_all_callbacks(self):
        """
        Removes all callbacks. Sub-histories will no longer be updated. 
        """
        pass

    def get_sub_history(self, filter_pred):
        """
        Returns an instance of TestHistory that contains only objects that satisfy the filter predicate.
        This sub-history will be updated automatically when new objects are added to the parent history, 
        however, the sub-history must be manually cleared. Sub-histories can be used to more efficiently 
        search for common events or telemetry in an F' deployment. This functionality is built in to 
        histories, but not used by the default API.  
        :param filter_pred: The predicate that will be used to narrow the history. This predicate is also used as a key to update this history.
        :return: A new TestHistory that is only updated with objects that satisfy the predicate 
        """
        pass


    ###################################################################################
    ##  Search Functions
    ###################################################################################
    def find_earliest(self, search_pred):
        """
        Searches for the first instance of an object that satisfies the given predicate in a history
        :param search_pred: A predicate to call on each object in the history
        :return: the first object to satisfy the given predicate
        """
        pass

    def find_latest(self, search_pred):
        """
        Searches for the last instance of an object that satisfies the given predicate in a history
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