"""
predicates.py:

This file contains basic predicates as well as event and telemetry predicates used by the
gds_test_api.py. The predicates are organized by type and can be used to search histories.

:author: koran
"""
from inspect import signature

from fprime_gds.common.data_types.ch_data import ChData
from fprime_gds.common.data_types.event_data import EventData


##########################################################################################
# Parent Class
##########################################################################################
class predicate:
    def __call__(self, item):
        """
        A parent class to ensure that all predicates are callable, and return readable strings
        """
        raise NotImplementedError(
            "This predicate did not override __call__(self, object)"
        )

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        raise NotImplementedError("This predicate did not override __str__(self)")


def is_predicate(pred):
    """
    a helper function to determine if an object can be used as a predicate.

    :return: a boolean value of whether the function is a predicate instance or has
        both __str__ and __call__ methods.
    """
    if isinstance(pred, predicate):
        return True
    if callable(pred):
        sig = signature(pred.__call__)
        arg_count = len(sig.parameters)
        if arg_count == 1:  # TODO weigh what value this should actually be.
            if hasattr(pred, "__str__"):
                return True
    return False


##########################################################################################
# Basic predicates
##########################################################################################
class less_than(predicate):
    def __init__(self, value):
        """
        A predicate that evaluates a less-than comparison
        :param value: To return true, the predicate must be called on an object that is less
            than this value
        """

        self.upper_limit = value

    def __call__(self, actual):
        """
        :param actual: the value to compare
        """
        return actual < self.upper_limit

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "This predicate evaluates if an item is less than {}".format(
            self.upper_limit
        )


class greater_than(predicate):
    def __init__(self, value):
        """
        A predicate that evaluates a greater-than comparison
        :param value: To return true, the predicate must be called on an object that is less
            than this value
        """
        self.lower_limit = value

    def __call__(self, actual):
        """
        :param actual: the value to compare
        """
        return actual > self.lower_limit

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "This predicate evaluates if an item is greater than {}".format(
            self.lower_limit
        )


class equal_to(predicate):
    def __init__(self, value):
        """
        A predicate that evaluates an equivalent comparison
        :param value: To return true, the predicate must be called on an object that is equal
            to this value
        """
        self.expected = value

    def __call__(self, actual):
        """
        :param actual: the value to compare
        """
        return actual == self.expected

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "This predicate evaluates if an item is equal to {}".format(
            self.expected
        )


class not_equal_to(predicate):
    def __init__(self, value):
        """
        A predicate that evaluates a not-equivalent comparison
        :param value: To return true, the predicate must be called on an object that is not
            equal to this value
        """
        self.expected = value

    def __call__(self, actual):
        """
        :param actual: the value to compare
        """
        return actual != self.expected

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "This predicate evaluates if an item is not equal to {}.".format(
            self.expected
        )


class less_than_or_equal_to(predicate):
    def __init__(self, value):
        """
        A predicate that evaluates a less-than-or-equals comparison
        :param value: To return true, the predicate must be called on an object that is less
            than or equal to this value
        """
        self.upper_limit = value

    def __call__(self, actual):
        """
        :param actual: the value to compare
        """
        return actual <= self.upper_limit

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "This predicate evaluates if an item is at most {}.".format(
            self.upper_limit
        )


class greater_than_or_equal_to(predicate):
    def __init__(self, value):
        """
        A predicate that evaluates greater-than-or-equals comparison
        :param value: To return true, the predicate must be called on an object that is
            greater than or equal to this value
        """
        self.lower_limit = value

    def __call__(self, actual):
        """
        :param actual: the value to compare
        """
        return actual >= self.lower_limit

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "This predicate evaluates if an item is at least {}.".format(
            self.lower_limit
        )


class within_range(predicate):
    def __init__(self, lower, upper):
        """
        A predicate that evaluates if the argument is between the two values
        :param lower: To return true, the predicate must be called on an object that is
            greater than this value
        :param upper: To return true, the predicate must be called on an object that is less
            than this value
        """
        self.upper_limit = upper
        self.lower_limit = lower

    def __call__(self, actual):
        """
        :param actual: the value to evaluate
        """
        return actual >= self.lower_limit and actual <= self.upper_limit

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "This predicate evaluates if an item is within {} and {}.".format(
            self.lower_limit, self.upper_limit
        )


##########################################################################################
# Set predicates
##########################################################################################
class is_a_member_of(predicate):
    def __init__(self, collection):
        """
        Constructs a predicate that evaluates if the argument is equivalent to any member in the set
        :param collection: To return true, the predicate must be called on an object that is
            equivalent to any object in this list
        """
        self.set = collection

    def __call__(self, item):
        """
        Evaluates the predicate
        :param item: the object to search for then evaluate
        """
        for x in self.set:
            if item == x:
                return True
        return False

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "This predicate evaluates if an item is a member of {}.".format(self.set)


class is_not_a_member_of(predicate):
    def __init__(self, collection):
        """
        Constructs a predicate that evaluates if the argument is not equivalent to all members in the set
        :param collection: To return true, the predicate must be called on an object that is
            not equivalent to any object in this collection
        """
        self.set = collection

    def __call__(self, item):
        """
        :param item: the object to search for then evaluate
        """
        for x in self.set:
            if item == x:
                return False
        return True

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "This predicate evaluates if an item is not a member of {}.".format(
            self.set
        )


##########################################################################################
# Logic predicates
##########################################################################################
class always_true(predicate):
    def __call__(self, object):
        """
        used as a placeholder by other predicates. This is like a logical TRUE signal.
        :param object: the object or value to evaluate
        """
        return True

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "This predicate always evaluates True."


class invert(predicate):
    def __init__(self, pred):
        """
        A predicate that negates a given predicate. This predicate can be used like a NOT
        gate when combining predicates.
        :param pred: The predicate to be negated.
        """
        if is_predicate(pred):
            self.pred = pred

    def __call__(self, item):
        """
        :param item: the object or value to evaluate
        """
        return not self.pred(item)

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "This predicate inverts the value of {}.".format(self.pred)


class satisfies_all(predicate):
    def __init__(self, pred_list):
        """
        A predicate that evaluates if the argument satisfies all predicates in the given list.
        This predicate can be used like an AND gate of N elements when combining predicates.
        :param pred_list: a list of predicates
        """
        self.p_list = []
        for pred in pred_list:
            if is_predicate(pred):
                self.p_list.append(pred)

    def __call__(self, item):
        """
        :param item: the object or value to evaluate
        """
        for pred in self.p_list:
            if not pred(item):
                return False
        return True

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "This predicate evaluates if an item satisfies all predicates in the list {}.".format(
            self.p_list
        )


class satisfies_any(predicate):
    def __init__(self, pred_list):
        """
        A predicate that evaluates if the argument satisfies any predicate in the given list.
        This predicate can be used like an OR gate of N elements when combining predicates.
        :param pred_list: a list of predicates
        """
        self.p_list = []
        for pred in pred_list:
            if is_predicate(pred):
                self.p_list.append(pred)

    def __call__(self, item):
        """
        :param item: the object or value to evaluate
        """
        for pred in self.p_list:
            if pred(item):
                return True
        return False

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "This predicate evaluates if an item satisfies any predicates in the list {}.".format(
            self.p_list
        )


##########################################################################################
# Test API predicates
##########################################################################################
class args_predicate(predicate):
    def __init__(self, args):
        """
        A predicate for evaluating argument fields. Arguments can be specified by value, by
        predicate or as don't care (None). By inserting None into the argument list, args_predicate
        will accept any response given for that argument index.
        
        Args:
            args: a list of expected arguments (list of values, predicates and None)
        """
        self.arg_spec = []
        ignored = always_true()
        if not isinstance(args, list):
            args = [args]
        for arg in args:
            if arg is None:
                self.arg_spec.append(ignored)
            elif is_predicate(arg):
                self.arg_spec.append(arg)
            else:
                self.arg_spec.append(equal_to(arg))

    def __call__(self, actual):
        """
        Determines if the given array satisfies the given argument specification.
        """
        if not isinstance(actual, list):
            actual = [actual]
        if len(actual) != len(self.arg_spec):
            return False
        for i in range(len(self.arg_spec)):
            if not self.arg_spec[i](actual[i]):
                return False
        return True

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "This predicate evaluates if an argument list satisfies a given set of specifiers: {}.".format(
            self.args
        )


class event_predicate(predicate):
    def __init__(self, id_pred=None, args_pred=None, time_pred=None):
        """
        A predicate for specifying an EventData object from data_types.event_data. This predicate
        can be used to search a history. If arguments passed into this constructor are not
        subclasses of predicate, they will be ignored. If an argument is unspecified, the predicate
        will ignore that field when evaluating an EventData object.

        Args:
            id_pred: optional predicate to call on the EventData instance's id field
            args_pred: optional predicate to call on the EventData instance's arguments
            time_pred: optional predicate to call on the EventData instance's timestamp
        """
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

    def __call__(self, event):
        """
        The event_predicate checks that the telemetry object is an instance of EventData and will
        raise an error if the check fails. Then event_predicate will evaluate whether event's
        EventData fields satisfy the id_pred, value_pred, and time_pred specified.

        Args:
            event: an instance of EventData (object)
        """
        if not isinstance(event, EventData):
            return False
        e_id = self.id_pred(event.get_id())
        e_args = self.args_pred(event.get_args())
        e_time = self.time_pred(event.get_time().useconds)
        return e_id and e_args and e_time

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        raise NotImplementedError("This predicate did not override __str__(self)")


class telemetry_predicate(predicate):
    def __init__(self, id_pred=None, value_pred=None, time_pred=None):
        """
        A predicate for specifying a ChData object from data_types.ch_data. This predicate can be
        used to search a history. If arguments passed into this constructor are not subclasses of
        predicate, they will be ignored.If an argument is unspecified, the predicate will ignore
        that field when evaluating an EventData object.

        Args:
            id_pred: optional predicate to call on the ChData instance's id field
            value_pred: optional predicate to call on the ChData instance's id field
            time_pred: optional predicate to call on the ChData instance's timestamp
        """
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

    def __call__(self, telemetry):
        """
        The telemetry_predicate checks that the telemetry object is an instance of ChData and will
        raise an error if the check fails. Then telemetry_predicate will evaluate whether
        telemetry's ChData fields satisfy the id_pred, value_pred and time_pred specified.

        Args:
            telemetry: an instance of ChData (object)
        """
        if not isinstance(telemetry, ChData):
            return False
        t_id = self.id_pred(telemetry.get_id())
        t_val = self.value_pred(telemetry.get_val())
        t_time = self.time_pred(telemetry.get_time().useconds)
        return t_id and t_val and t_time

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        raise NotImplementedError("This predicate did not override __str__(self)")
