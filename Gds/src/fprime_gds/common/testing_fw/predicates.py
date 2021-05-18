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
# Base class/helpers
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

    def __repr__(self):
        """
        Unless manually overwritten by a subclass both repr and str will return the same string.
        """
        return str(self)


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
        if arg_count == 1:
            if hasattr(pred, "__str__"):
                return True
    return False


def get_descriptive_string(value, pred_function):
    """
    a helper function that formats a predicate and argument in a nice human-readable format

    Args:
        value: the argument of the predicate
        pred_function: a predicate function
    """
    return "F({}), where F(x) evaluates\n\t {}".format(value, pred_function)


##########################################################################################
# Comparison predicates
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
        try:
            return actual < self.upper_limit
        except TypeError:
            return False

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "x < {}".format(self.upper_limit)


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
        try:
            return actual > self.lower_limit
        except TypeError:
            return False

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "x > {}".format(self.lower_limit)


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
        try:
            return actual == self.expected
        except TypeError:
            return False

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "x == {}".format(self.expected)


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
        try:
            return actual != self.expected
        except TypeError:
            return False

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "x != {}".format(self.expected)


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
        try:
            return actual <= self.upper_limit
        except TypeError:
            return False

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "x <= {}".format(self.upper_limit)


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
        try:
            return actual >= self.lower_limit
        except TypeError:
            return False

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "x >= {}".format(self.lower_limit)


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
        try:
            return self.lower_limit <= actual <= self.upper_limit
        except TypeError:
            return False

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "{} <= x <= {}".format(self.lower_limit, self.upper_limit)


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
        return "x \u2208 {}".format(self.set)


class is_not_a_member_of(predicate):
    def __init__(self, collection):
        """
        Constructs a predicate that evaluates if the argument is not equivalent to all members in
        the set
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
        return "x \u2209 {}".format(self.set)


##########################################################################################
# Logic predicates
##########################################################################################
class always_true(predicate):
    def __call__(self, obj):
        """
        used as a placeholder by other predicates. This is like a logical TRUE signal.
        :param obj: the object or value to evaluate
        """
        return True

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "True"


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
        return "\u02DC({}).".format(self.pred)


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
        return "\u2200 P \u2208 A : P(x) is True, when A is {}".format(self.p_list)


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
        return "\u2203 P \u2208 A : P(x) is True, when A is {}".format(self.p_list)


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
            args: a list of expected arguments (list of values, predicates, and None)
        """
        self.arg_spec = []
        ignored = always_true()
        if not isinstance(args, (list, tuple)):
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
        if not isinstance(actual, (list, tuple)):
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
        return "True IFF \u2200 pi \u2208 P and xi \u2208 x; pi(xi) is True. Where P is {}".format(
            self.arg_spec
        )


class event_predicate(predicate):
    def __init__(
        self, id_pred=None, args_pred=None, severity_pred=None, time_pred=None
    ):
        """
        A predicate for specifying an EventData object from data_types.event_data. This predicate
        can be used to search a history. If arguments passed into this constructor are not
        subclasses of predicate, they will be ignored. If an argument is unspecified, the predicate
        will ignore that field when evaluating an EventData object.

        Args:
            id_pred: optional predicate to call on the EventData instance's id field
            args_pred: optional predicate to call on a list of the EventData 's argument values
            severity_pred: optional predicate to call on the EventData instances severity field
            time_pred: optional predicate to call on the EventData instance's timestamp
        """
        true_pred = always_true()
        self.id_pred = true_pred
        self.args_pred = true_pred
        self.severity_pred = true_pred
        self.time_pred = true_pred
        if is_predicate(id_pred):
            self.id_pred = id_pred
        if is_predicate(args_pred):
            self.args_pred = args_pred
        if is_predicate(severity_pred):
            self.severity_pred = severity_pred
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
        if self.id_pred(event.get_id()):
            if self.time_pred(event.get_time()):
                if self.severity_pred(event.get_severity()):
                    args = []
                    for arg in event.get_args():
                        args.append(arg.val)
                    if self.args_pred(args):
                        return True
        return False

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        msg = "True IFF: x is an EventData object"
        if not isinstance(self.id_pred, always_true):
            msg += " and x's id satisfies ({})".format(self.id_pred)
        if not isinstance(self.args_pred, always_true):
            msg += " and x's args satisfy ({})".format(self.args_pred)
        if not isinstance(self.severity_pred, always_true):
            msg += " and x's severity satisfies ({})".format(self.severity_pred)
        if not isinstance(self.time_pred, always_true):
            msg += " and x's time satisfies ({})".format(self.time_pred)
        return msg


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
        if self.id_pred(telemetry.get_id()):
            if self.value_pred(telemetry.get_val()):
                if self.time_pred(telemetry.get_time()):
                    return True
        return False

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        msg = "True IFF: x is a ChData object"
        if not isinstance(self.id_pred, always_true):
            msg += " and x's id satisfies ({})".format(self.id_pred)
        if not isinstance(self.value_pred, always_true):
            msg += " and x's value satisfies ({})".format(self.value_pred)
        if not isinstance(self.time_pred, always_true):
            msg += " and x's time satisfies ({})".format(self.time_pred)
        return msg
