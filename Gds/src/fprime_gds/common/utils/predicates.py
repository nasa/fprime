"""
predicates.py:

This file contains basic predicates as well as event and telemetry predicates used by the
gds_test_api.py. The predicates are organized by type and can be used to search histories.

:author: koran
"""
from inspect import signature

from data_types.event_data import EventData
from data_types.ch_data import ChData


##########################################################################################
# Parent Class
##########################################################################################
class predicate:
    """
    A parent class to ensure that all predicates are callable, and return readable strings
    """
    def __call__(self, object):
        # TODO raise not-implemented error
        pass
    """
    Returns a string outlining the evaluation done by the predicate.
    """
    def __str__(self, object):
        # TODO raise not-implemented error
        pass


def is_predicate(pred):
    """
    a helper function to determine if an object can be used as a predicate.

    :return: a boolean value of whether the function is a predicate instance or has
        __str__ and __call__ methods.
    """
    if isinstance(pred, predicate):
        return True
    if callable(pred):
        sig = signature(pred.__call__)
        arg_count = len(sig.parameters)
        if arg_count == 1:  # TODO weigh what value this should actually be.
            if hasattr(pred, '__str__'):
                return True
    return False


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
        self.upper_limit = value

    """
    :param actual: the value to compare
    """
    def __call__(self, actual):
        return actual < self.upper_limit

    """
    Returns a string outlining the evaluation done by the predicate.
    """
    def __str__(self, object):
        # TODO return a string
        pass


class greater_than(predicate):
    """
    A predicate that evaluates a greater-than comparison
    :param value: To return true, the predicate must be called on an object that is less
        than this value
    """
    def __init__(self, value):
        self.lower_limit = value

    """
    :param actual: the value to compare
    """
    def __call__(self, actual):
        return actual > self.lower_limit

    """
    Returns a string outlining the evaluation done by the predicate.
    """
    def __str__(self, object):
        # TODO return a string
        pass


class equal_to(predicate):
    """
    A predicate that evaluates an equivalent comparison
    :param value: To return true, the predicate must be called on an object that is equal
        to this value
    """
    def __init__(self, value):
        self.expected = value

    """
    :param actual: the value to compare
    """
    def __call__(self, actual):
        return actual == self.expected

    """
    Returns a string outlining the evaluation done by the predicate.
    """
    def __str__(self, object):
        # TODO return a string
        pass


class not_equal_to(predicate):
    """
    A predicate that evaluates a not-equivalent comparison
    :param value: To return true, the predicate must be called on an object that is not
        equal to this value
    """
    def __init__(self, value):
        self.expected = value

    """
    :param actual: the value to compare
    """
    def __call__(self, actual):
        return actual != self.expected

    """
    Returns a string outlining the evaluation done by the predicate.
    """
    def __str__(self, object):
        # TODO return a string
        pass


class less_than_or_equals(predicate):
    """
    A predicate that evaluates a less-than-or-equals comparison
    :param value: To return true, the predicate must be called on an object that is less
        than or equal to this value
    """
    def __init__(self, value):
        self.upper_limit = value

    """
    :param actual: the value to compare
    """
    def __call__(self, actual):
        return actual <= self.upper_limit

    """
    Returns a string outlining the evaluation done by the predicate.
    """
    def __str__(self, object):
        # TODO return a string
        pass


class greater_than_or_equals(predicate):
    """
    A predicate that evaluates greater-than-or-equals comparison
    :param value: To return true, the predicate must be called on an object that is
        greater than or equal to this value
    """
    def __init__(self, value):
        self.lower_limit = value

    """
    :param actual: the value to compare
    """
    def __call__(self, actual):
        return actual >= self.lower_limit

    """
    Returns a string outlining the evaluation done by the predicate.
    """
    def __str__(self, object):
        # TODO return a string
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
        self.upper_limit = upper
        self.lower_limit = lower

    """
    :param actual: the value to evaluate
    """
    def __call__(self, actual):
        return actual >= self.lower_limit and actual <= self.upper_limit

    """
    Returns a string outlining the evaluation done by the predicate.
    """
    def __str__(self, object):
        # TODO return a string
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
        self.set = []
        self.set.append(collection)

    """
    :param item: the object to search for then evaluate
    """
    def __call__(self, item):
        for x in self.set:
            if item == x:
                return True
        return False

    """
    Returns a string outlining the evaluation done by the predicate.
    """
    def __str__(self, object):
        # TODO return a string
        pass


class is_not_a_member_of(predicate):
    """
    A predicate that evaluates if the argument is not equivalent to all members in the set
    :param collection: To return true, the predicate must be called on an object that is
        not equivalent to any object in this collection
    """
    def __init__(self, collection):
        self.set = []
        self.set.append(collection)

    """
    :param item: the object to search for then evaluate
    """
    def __call__(self, item):
        for x in self.set:
            if item == x:
                return False
        return True

    """
    Returns a string outlining the evaluation done by the predicate.
    """
    def __str__(self, object):
        # TODO return a string
        pass


##########################################################################################
# Logic predicates
##########################################################################################
class always_true(predicate):
    """
    used as a placeholder by other predicates. This is like a logical TRUE signal.

    :param object: the object or value to evaluate
    """
    def __call__(self, object):
        return True

    """
    Returns a string outlining the evaluation done by the predicate.
    """
    def __str__(self, object):
        # TODO return a string
        pass


class invert(predicate):
    """
    A predicate that negates a given predicate. This predicate can be used like a NOT
    gate when combining predicates.
    :param pred: The predicate to be negated.
    """
    def __init__(self, pred):
        if is_predicate(pred):
            self.pred = pred

    """
    :param item: the object or value to evaluate
    """
    def __call__(self, item):
        return not self.pred(item)

    """
    Returns a string outlining the evaluation done by the predicate.
    """
    def __str__(self, object):
        # TODO return a string
        pass


class satisfies_all(predicate):
    """
    A predicate that evaluates if the argument satisfies all predicates in the given list.
    This predicate can be used like an AND gate of N elements when combining predicates.
    :param pred_list: a list of predicates
    """
    def __init__(self, pred_list):
        self.p_list = []
        for pred in pred_list:
            if is_predicate(pred):
                self.p_list.append(pred)

    """
    :param item: the object or value to evaluate
    """
    def __call__(self, item):
        for pred in self.p_list:
            if not pred(item):
                return False
        return True

    """
    Returns a string outlining the evaluation done by the predicate.
    """
    def __str__(self, object):
        # TODO return a string
        pass


class satisfies_any(predicate):
    """
    A predicate that evaluates if the argument satisfies any predicate in the given list.
    This predicate can be used like an OR gate of N elements when combining predicates.
    :param pred_list: a list of predicates
    """
    def __init__(self, pred_list):
        self.p_list = []
        for pred in pred_list:
            if is_predicate(pred):
                self.p_list.append(pred)

    """
    :param item: the object or value to evaluate
    """
    def __call__(self, item):
        for pred in self.p_list:
            if pred(item):
                return True
        return False

    """
    Returns a string outlining the evaluation done by the predicate.
    """
    def __str__(self, object):
        # TODO return a string
        pass


##########################################################################################
# Test API predicates
##########################################################################################
class args_predicate(predicate):
    """
    A predicate for evaluating argument fields.
    :param args: a list of expected arguments.
    """
    def __init__(self, args):
        self.expected = args

    """
    Evaluates if the given argument array is equivalent. If a given argument is none, it
    will be ignored.
    """
    def __call__(self, actual):
        for arg in self.expected:
            # TODO finish impelmentation
            pass

    """
    Returns a string outlining the evaluation done by the predicate.
    """
    def __str__(self, object):
        # TODO return a string
        pass


class event_predicate(predicate):
    """
    A predicate for specifying an EventData object from data_types.event_data. This
    predicate can be used to search a history. If arguments passed into this constructor
    are not subclasses of predicate, they will be ignored.

    :param id_pred: If specified, the object's id field must satisfy the given predicate
        for the telemetry predicate to evaluate to true.
    :param args_pred: If specified, the object's arguments field must satisfy the given
        predicate for the telemetry predicate to evaluate to true.
    :param time_pred: If specified, the object's time field must satisfy the given
        predicate for the telemetry predicate to evaluate to true.
    """
    def __init__(self, id_pred=None, args_pred=None, time_pred=None):
        true_pred = always_true()
        self.id_pred = true_pred
        self.args_pred = true_pred
        self.time_pred = true_pred
        if is_predicate(id_pred):
            self.id_pred = id_pred
        if is_predicate(args_pred):
            self.args_pred = args_pred
        if is_predicate(time_pred):
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
        e_id = self.id_pred(event.get_id())
        e_args = self.args_pred(event.get_args())
        e_time = self.time_pred(event.get_time())
        return e_id and e_args and e_time

    """
    Returns a string outlining the evaluation done by the predicate.
    """
    def __str__(self, object):
        # TODO return a string
        pass


class telemetry_predicate(predicate):
    """
    A predicate for specifying a ChData object from data_types.ch_data. This predicate
    can be used to search a history. If arguments passed into this constructor are not
    subclasses of predicate, they will be ignored.

    :param id_pred: If specified, the object's id field must satisfy the given predicate
        for the telemetry predicate to evaluate to true.
    :param Value_pred: If specified, the object's value_obj field must satisfy the given
        predicate for the telemetry predicate to evaluate to true.
    :param time_pred: If specified, the object's time field must satisfy the given
        predicate for the telemetry predicate to evaluate to true.
    """
    def __init__(self, id_pred=None, value_pred=None, time_pred=None):
        true_pred = always_true()
        self.id_pred = true_pred
        self.value_pred = true_pred
        self.time_pred = true_pred
        if is_predicate(id_pred):
            self.id_pred = id_pred
        if is_predicate(value_pred):
            self.value_pred = value_pred
        if is_predicate(time_pred):
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
        t_id = self.id_pred(telemetry.get_id())
        t_val = self.value_pred(telemetry.get_val())
        t_time = self.time_pred(telemetry.get_time())
        return t_id and t_val and t_time

    """
    Returns a string outlining the evaluation done by the predicate.
    """
    def __str__(self, object):
        # TODO return a string
        pass
