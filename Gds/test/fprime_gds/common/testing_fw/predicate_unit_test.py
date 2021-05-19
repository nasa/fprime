import os
import sys
import unittest

from fprime.common.models.serialize.numerical_types import I32Type
from fprime.common.models.serialize.string_type import StringType
from fprime.common.models.serialize.time_type import TimeType
from fprime_gds.common.data_types.ch_data import ChData
from fprime_gds.common.data_types.event_data import EventData
from fprime_gds.common.templates.ch_template import ChTemplate
from fprime_gds.common.templates.event_template import EventTemplate
from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.utils.event_severity import EventSeverity

filename = os.path.dirname(__file__)
gdsName = os.path.join(filename, "../../../../src")
fprimeName = os.path.join(filename, "../../../../../Fw/Python/src")
sys.path.insert(0, gdsName)
sys.path.insert(0, fprimeName)


class PredicateTestCases(unittest.TestCase):
    @staticmethod
    def check_str(pred):
        """
        Tests for the predicate class and it's functionality.
        """
        try:
            pred.__str__()
            print(pred)
            assert True, "predicate provides string summary: {}".format(str(pred))
        except NotImplementedError:
            assert False, "invoking str(pred) was not supported"

    @staticmethod
    def test_NotImplemented():
        class dummyPred(predicates.predicate):
            pass

        pred = dummyPred()

        try:
            pred.__call__(2)
            assert (
                False
            ), "invoking an incomplete subclass didn't raise NotImplementedError"
        except NotImplementedError:
            assert True, "invoking an incomplete subclass raised NotImplementedError"

        try:
            str(pred)
            assert (
                False
            ), "invoking an incomplete subclass didn't raise NotImplementedError"
        except NotImplementedError:
            assert True, "invoking an incomplete subclass raised NotImplementedError"

    @staticmethod
    def test_Implemented():
        pred = predicates.less_than(2)
        try:
            pred.__call__(2)
        except NotImplementedError:
            assert (
                False
            ), "invoking __call__ on an complete subclass of predicate failed"

        try:
            str(pred)
        except NotImplementedError:
            assert False, "invoking __str__ on an complete subclass of predicate failed"
        assert True, "implemented predicate had no problems invoking functions"

    @staticmethod
    def test_is_predicate():
        class dummyPred:
            def __call__(self, item):
                pass

            def __str__(self):
                return "dummyPred"

        pred = dummyPred()
        assert predicates.is_predicate(
            pred
        ), "a class with call and str methods satisfies is_predicate()"
        pred = object()
        assert not predicates.is_predicate(
            pred
        ), "a class without call and str doesn't satisfy is_predicate()"
        pred = predicates.predicate()
        assert predicates.is_predicate(
            pred
        ), "an instance of predicate satisfies is_predicate()"
        pred = predicates.equal_to(1)
        assert predicates.is_predicate(
            pred
        ), "an instance of a subclass of predicate satisfies is_predicate()"

    def test_less_than(self):
        pred = predicates.less_than(2)
        assert pred(1), "one is less than two"
        assert pred(-1), "negative one is less than two"
        assert not pred(2), "two is not less than two"
        assert not pred(3), "three is not less than two"
        self.check_str(pred)

    def test_greater_than(self):
        pred = predicates.greater_than(2)
        assert pred(3), "three is greater than two"
        assert pred(100), "one hundred is greater than two"
        assert not pred(2), "two is not greater than two"
        assert not pred(-1), "negative one is not greater than two"
        self.check_str(pred)

    def test_equal_to(self):
        pred = predicates.equal_to(1)
        assert pred(1), "one is equal to one"
        assert not pred(100), "one hundred is not equal to one"
        assert not pred(2), "two is not equal to one"
        assert not pred(-1), "negative one is not equal to one"
        self.check_str(pred)

    def test_not_equal_to(self):
        pred = predicates.not_equal_to(1)
        assert pred(0), "zero is not equal to one"
        assert pred(-1), "negative one is not equal to one"
        assert pred(2), "two is not equal to one"
        assert not pred(1), "one is not not equal to one"
        self.check_str(pred)

    def test_less_than_or_equal_to(self):
        pred = predicates.less_than_or_equal_to(1)
        assert pred(1), "one is less than or equal to 1"
        assert pred(-1), "negative one is less than or equal to 1"
        assert not pred(10), "ten is not less than or equal to 1"
        self.check_str(pred)

    def test_greater_than_or_equal_to(self):
        pred = predicates.greater_than_or_equal_to(1)
        assert pred(1), "one is greater than or equal to 1"
        assert pred(10), "ten is greater than or equal to 1"
        assert not pred(-1), "negative is not greater than or equal to 1"
        self.check_str(pred)

    def test_within_range(self):
        pred = predicates.within_range(5, 10)
        assert pred(5), "5 is the lower bound and within [5,10]"
        assert pred(10), "10 is the upper bound and within [5,10]"
        assert pred(7.5), "7.5 is within [5,10]"
        assert not pred(2), "2 is not within [5,10]"
        assert not pred(11), "2 is not within [5,10]"
        self.check_str(pred)

    def test_is_a_member_of(self):
        pred = predicates.is_a_member_of(["a", "b", "c"])
        assert pred("a"), "a is in the set [a, b, c]"
        assert pred("b"), "b is in the set [a, b, c]"
        assert pred("c"), "c is in the set [a, b, c]"
        assert not pred(1), "1 is not in the set [a, b, c]"
        assert not pred("x"), "x is not in the set [a, b, c]"
        self.check_str(pred)

    def test_is_not_a_member_of(self):
        pred = predicates.is_not_a_member_of(["a", "b", "c"])
        assert pred(1), "1 is not in the set [a, b, c]"
        assert pred("x"), "x is not in the set [a, b, c]"
        assert not pred("a"), "a is not not in the set [a, b, c]"
        assert not pred("b"), "b is not not in the set [a, b, c]"
        assert not pred("c"), "c is not not in the set [a, b, c]"
        self.check_str(pred)

    def test_always_true(self):
        pred = predicates.always_true()
        assert pred(1), "numbers are true"
        assert pred("string"), "strings are true"
        assert pred(object()), "an object is true"
        self.check_str(pred)

    def test_invert(self):
        pred = predicates.invert(predicates.always_true())
        assert not pred(1), "numbers are not true"
        assert not pred("string"), "strings are not true"
        assert not pred(object()), "an object is not true"
        self.check_str(pred)

    def test_satisfies_all(self):
        p_list = list()
        p_list.append(predicates.less_than(8))
        p_list.append(predicates.less_than(6))
        p_list.append(predicates.equal_to(4))
        pred = predicates.satisfies_all(p_list)
        assert pred(4), "4 satisfies all predicates in the list"
        assert not pred(5), "5 satisfies only 2 predicates in the list"
        assert not pred(7), "7 satisfies only 1 predicate in the list"
        assert not pred(9), "9 satisfies only no predicates in the list"
        self.check_str(pred)

    def test_satisfies_any(self):
        p_list = list()
        p_list.append(predicates.less_than(8))
        p_list.append(predicates.less_than(6))
        p_list.append(predicates.equal_to(4))
        pred = predicates.satisfies_any(p_list)
        assert pred(4), "4 satisfies all predicates in the list"
        assert pred(5), "5 satisfies only 2 predicates in the list"
        assert pred(7), "7 satisfies only 1 predicate in the list"
        assert not pred(9), "9 satisfies only no predicates in the list"
        self.check_str(pred)

    @staticmethod
    def test_args_predicates():
        a_list = ["a", "p", "p", "l", "e"]
        pred = predicates.args_predicate(["a", "p", "p", "l", "e"])
        assert pred(a_list), "The list {} should have been accepted".format(a_list)
        a_list[4] = "r"
        assert not pred(a_list), "The list {} should not have been accepted".format(
            a_list
        )
        a_list = ["a", "p", "p", "l"]
        assert not pred(a_list), "The list {} should not have been accepted".format(
            a_list
        )

        a_list = ["a", "p", "p", "l", "e"]
        pred = predicates.args_predicate(["a", "p", "p", "l", None])
        assert pred(a_list), "The list {} should have been accepted".format(a_list)
        a_list[4] = 7
        assert pred(a_list), "The list {} should have been accepted".format(a_list)
        a_list[4] = "r"
        assert pred(a_list), "The list {} should have been accepted".format(a_list)

        l_pred = predicates.within_range(0, 10)
        pred = predicates.args_predicate([l_pred, 2, 3, 4, 5, 6])

        n_list = [1, 2, 3, 4, 5, 6]
        assert pred(n_list), "The list {} should have been accepted".format(n_list)

        for i in range(0, 10):
            n_list[0] = i
            assert pred(n_list), "The list {} should have been accepted".format(n_list)
        n_list[0] = -5
        assert not pred(n_list), "The list {} should not have been accepted".format(
            n_list
        )
        n_list[0] = 15
        assert not pred(n_list), "The list {} should not have been accepted".format(
            n_list
        )

        pred = predicates.args_predicate(8)
        assert pred(8), "The value 8 should have been accepted."

    def test_telemetry_predicates(self):
        temp1 = ChTemplate(1, "Test Channel 1", "Predicate_Tester", I32Type())
        temp2 = ChTemplate(2, "Test Channel 2", "Predicate_Tester", StringType())
        update1 = ChData(I32Type(20), TimeType(), temp1)
        update2 = ChData(StringType("apple"), TimeType(), temp2)

        pred = predicates.telemetry_predicate()
        assert pred(
            update1
        ), "If no fields are specified a ChData object should return True"
        assert pred(
            update2
        ), "If no fields are specified a ChData object should return True"
        assert not pred(
            "diff object"
        ), "Anything that's not a ChData object should be False"
        assert not pred(5), "Anything that's not a ChData object should be False"
        self.check_str(pred)

        id_pred = predicates.equal_to(1)
        pred = predicates.telemetry_predicate(id_pred=id_pred)
        assert pred(update1), "This predicate on the ID 1 should return True"
        assert not pred(update2), "This predicate on the ID 2 should return False"
        self.check_str(pred)

        val_pred = predicates.equal_to("apple")
        pred = predicates.telemetry_predicate(value_pred=val_pred)
        assert not pred(update1), "This predicate on the value 20 should return False"
        assert pred(update2), 'This predicate on the value "apple" should return True'
        self.check_str(pred)

        time_pred = predicates.equal_to(0)
        pred = predicates.telemetry_predicate(time_pred=time_pred)
        assert pred(update1), "This predicate on the time 0 should return True"
        assert pred(update2), "This predicate on the time 0 should return True"
        self.check_str(pred)

        val_pred = predicates.within_range(10, 30)
        pred = predicates.telemetry_predicate(id_pred, val_pred, time_pred)
        assert pred(update1), "Specifying all fields should return True for update 1"
        assert not pred(
            update2
        ), "Specifying all fields should return False for update 2"
        self.check_str(pred)

    def test_event_predicates(self):
        args1_def = [("name", "string", StringType()), ("age", "int", I32Type())]
        temp1 = EventTemplate(
            1,
            "Test Msg 1",
            "Predicate Tester",
            args1_def,
            EventSeverity.ACTIVITY_LO,
            "",
        )
        args1 = (StringType("John"), I32Type(35))
        msg1 = EventData(args1, TimeType(), temp1)
        args2_def = [
            ("description", "string", StringType()),
            ("count", "int", I32Type()),
        ]
        temp2 = EventTemplate(
            2,
            "Test Msg 2",
            "Predicate Tester",
            args2_def,
            EventSeverity.ACTIVITY_HI,
            "",
        )
        args2 = (StringType("Dozen"), I32Type(12))
        msg2 = EventData(args2, TimeType(), temp2)

        pred = predicates.event_predicate()
        assert pred(
            msg1
        ), "If no fields are specified an EventData object should return True"
        assert pred(
            msg2
        ), "If no fields are specified an EventData object should return True"
        assert not pred(
            "diff object"
        ), "Anything that's not an EventData object should be False"
        assert not pred(5), "Anything that's not a EventData object should be False"
        self.check_str(pred)

        id_pred = predicates.equal_to(1)
        pred = predicates.event_predicate(id_pred=id_pred)
        assert pred(msg1), "This predicate on the ID 1 should return True"
        assert not pred(msg2), "This predicate on the ID 2 should return False"
        self.check_str(pred)

        args_pred = predicates.args_predicate([None, None])
        pred = predicates.event_predicate(args_pred=args_pred)
        assert pred(
            msg1
        ), "This predicate should return True, as it expects an event with 2 args"
        assert pred(
            msg2
        ), "This predicate should return True, as it expects an event with 2 args"
        self.check_str(pred)

        args_pred = predicates.args_predicate(["John", 35])
        pred = predicates.event_predicate(args_pred=args_pred)
        assert pred(
            msg1
        ), "This predicate should return True as msg1 has args (str John, int32 35)"
        assert not pred(
            msg2
        ), "This predicate should return False as msg2 has args (str Dozen, int32 12)"
        self.check_str(pred)

        severity_pred = predicates.equal_to(EventSeverity.ACTIVITY_LO)
        pred = predicates.event_predicate(severity_pred=severity_pred)
        assert severity_pred(msg1.get_severity())
        assert pred(
            msg1
        ), "This predicate should return True as msg1 has an ACTIVITY_LO severity"
        assert not pred(
            msg2
        ), "This predicate should return False as msg2 has an ACTIVITY_HI severity"
        self.check_str(pred)

        time_pred = predicates.equal_to(0)
        pred = predicates.event_predicate(time_pred=time_pred)
        assert pred(msg1), "This predicate on the time 0 should return True"
        assert pred(msg2), "This predicate on the time 0 should return True"
        self.check_str(pred)

        pred = predicates.event_predicate(id_pred, args_pred, severity_pred, time_pred)
        assert pred(msg1), "Specifying all fields should return True for msg1"
        assert not pred(msg2), "Specifying all fields should return False for msg2"
        self.check_str(pred)


if __name__ == "__main__":
    unittest.main()
