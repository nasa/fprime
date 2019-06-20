"""
predicates.py:

This file contains basic predicates as well as event and telemetry predicates used by the
gds_test_api.py. The predicates are organized by type and can be used to search histories.

:author: koran
"""
from data_types.event_data import EventData
from data_types.ch_data import ChData

##########################################################################################
# Parent Class
##########################################################################################
class predicate:
    """
    A parent class to ensure that all predicates are callable
    """
    def __call__(self, object):
        # TODO raise not-implemented error
        pass

##########################################################################################
# Basic predicates
##########################################################################################


class less_than(predicate):
    """
    A predicate that evaluates a less-than comparison
    :param value: To return true, the predicate must be called on an object that is less
        than this value
    """
    def __init__(self, value):
        pass

    """
    :param actual: the value to compare
    """
    def __call__(self, actual):
        pass


class greater_than(predicate):
    """
    A predicate that evaluates a greater-than comparison
    :param value: To return true, the predicate must be called on an object that is less
        than this value
    """
    def __init__(self, value):
        pass

    """
    :param actual: the value to compare
    """
    def __call__(self, actual):
        pass


class equal_to(predicate):
    """
    A predicate that evaluates an equivalent comparison
    :param value: To return true, the predicate must be called on an object that is equal
        to this value
    """
    def __init__(self, value):
        pass

    """
    :param actual: the value to compare
    """
    def __call__(self, actual):
        pass


class not_equal_to(predicate):
    """
    A predicate that evaluates a not-equivalent comparison
    :param value: To return true, the predicate must be called on an object that is not
        equal to this value
    """
    def __init__(self, value):
        pass

    """
    :param actual: the value to compare
    """
    def __call__(self, actual):
        pass


class greater_than_or_equals(predicate):
    """
    A predicate that evaluates greater-than-or-equals comparison
    :param value: To return true, the predicate must be called on an object that is
        greater than or equal to this value
    """
    def __init__(self, value):
        pass

    """
    :param actual: the value to compare
    """
    def __call__(self, actual):
        pass


class less_than_or_equals(predicate):
    """
    A predicate that evaluates a less-than-or-equals comparison
    :param value: To return true, the predicate must be called on an object that is less
        than or equal to this value
    """
    def __init__(self, value):
        pass

    """
    :param actual: the value to compare
    """
    def __call__(self, actual):
        pass


class within_range(predicate):
    """
    A predicate that evaluates if the argument is between the two values
    :param lower: To return true, the predicate must be called on an object that is
        greater than this value
    :param upper: To return true, the predicate must be called on an object that is less
        than this value
    """
    def __init__(self, lower, upper):
        pass

    """
    :param actual: the value to evaluate
    """
    def __call__(self, actual):
        pass

##########################################################################################
# Set predicates
##########################################################################################


class is_a_member_of(predicate):
    """
    A predicate that evaluates if the argument is equivalent to any member in the set
    :param collection: To return true, the predicate must be called on an object that is
        equivalent to any object in this collection
    """
    def __init__(self, collection):
        pass

    """
    :param object: the object to search for then evaluate
    """
    def __call__(self, object):
        pass


class is_not_a_member_of(predicate):
    """
    A predicate that evaluates if the argument is not equivalent to all members in the set
    :param collection: To return true, the predicate must be called on an object that is
        not equivalent to any object in this collection
    """
    def __init__(self, collection):
        pass

    """
    :param object: the object to search for then evaluate
    """
    def __call__(self, object):
        pass


##########################################################################################
# Logic predicates
##########################################################################################

class invert(predicate):
    """
    A predicate that negates a given predicate. This predicate can be used like a NOT
    gate when combining predicates.
    :param pred: The predicate to be negated.
    """
    def __init__(self, pred):
        pass

    """
    :param object: the object or value to evaluate
    """
    def __call__(self, object):
        pass


class satisfies_all(predicate):
    """
    A predicate that evaluates if the argument satisfies all predicates in the given list.
    This predicate can be used like an AND gate of N elements when combining predicates.
    :param pred_list: a list of predicates
    """
    def __init__(self, pred_list):
        pass

    """
    :param object: the object or value to evaluate
    """
    def __call__(self, object):
        pass


class satisfies_any(predicate):
    """
    A predicate that evaluates if the argument satisfies any predicate in the given list.
    This predicate can be used like an OR gate of N elements when combining predicates.
    :param pred_list: a list of predicates
    """
    def __init__(self, pred_list):
        pass

    """
    :param object: the object or value to evaluate
    """
    def __call__(self, object):
        pass

##########################################################################################
# Test API predicates
##########################################################################################


class true_predicate(predicate):
    """
    used by event predicate and telemetry predicate as a placeholder when some predicates
    may not be specified.
    """
    def __init__(self):
        pass

    """
    :param object: the object or value to evaluate
    """
    def __call__(self, object):
        return True


class event_predicate(predicate):
    """
    A predicate for specifying an EventData object from data_types.event_data. This
    predicate can be used to search a history.

    :param id_pred: If specified, the object's id field must satisfy the given predicate
        for the telemetry predicate to evaluate to true.
    :param id_value: If specified, the object's value_obj field must satisfy the given
        predicate for the telemetry predicate to evaluate to true.
    :param time_pred: If specified, the object's time field must satisfy the given
        predicate for the telemetry predicate to evaluate to true.
    """
    def __init__(self, id_pred=None, args_pred=None, time_pred=None):
        self.id_p = id_pred
        self.args_pred = args_pred
        self.time_pred = time_pred

    """
    The event_predicate checks that the telemetry object is an instance of
    EventData and will raise an error if the check fails. Then
    event_predicate will evaluate whether event's EventData fields satisfy the
    id_pred, value_pred, and time_pred specified.

    :param event: an instance of EventData
    """
    def __call__(self, event):
        if(not isinstance(event, EventData)):
            pass  # TODO raise test error.
        e_id = self.id_p(event.get_id())
        e_args = self.args_pred(event.get_args())
        e_time = self.time_pred(event.get_time())
        return e_id and e_args and e_time


class telemetry_predicate(predicate):
    """
    A predicate for specifying a ChData object from data_types.ch_data. This predicate
    can be used to search a history.
    :param id_pred: If specified, the object's id field must satisfy the given predicate
        for the telemetry predicate to evaluate to true.
    :param id_value: If specified, the object's value_obj field must satisfy the given
        predicate for the telemetry predicate to evaluate to true.
    :param time_pred: If specified, the object's time field must satisfy the given
        predicate for the telemetry predicate to evaluate to true.
    """
    def __init__(self, id_pred=None, value_pred=None, time_pred=None):
        self.id_p = id_pred
        self.value_pred = value_pred
        self.time_pred = time_pred

    """
    The telemetry_predicate checks that the telemetry object is an instance of
    ChData and will raise an error if the check fails. Then
    telemetry_predicate will evaluate whether telemetry's ChData fields
    satisfy the id_pred, value_pred and time_pred specified.
    :param telemetry: an instance of ChData
    """
    def __call__(self, telemetry):
        if(not isinstance(telemetry, ChData)):
            pass  # TODO raise test error.
        t_id = self.id_p(telemetry.get_id())
        t_val = self.value_pred(telemetry.get_val())
        t_time = self.time_pred(telemetry.get_time())
        return t_id and t_args and t_time
