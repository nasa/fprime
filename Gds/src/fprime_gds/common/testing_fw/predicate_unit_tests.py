import unittest
import sys
import os
filename = os.path.dirname(__file__)
gdsName = os.path.join(filename, '../../../')
fprimeName = os.path.join(filename, '../../../../../Fw/Python/src')
sys.path.insert(0, gdsName)
sys.path.insert(0, fprimeName)

from fprime_gds.common.testing_fw import predicates


class PredicateTestCases(unittest.TestCase):
    def check_str(self, pred):
        """
        Tests for the predicate class and it's functionality.
        """
        try:
            pred.__str__()
            assert True, "predicate provides string summary: {}".format(str(pred))
        except NotImplementedError:
            assert False, "invoking str(pred) was not supported"

    def test_NotImplemented(self):
        class dummyPred(predicates.predicate):
            pass
        pred = dummyPred()

        try:
            pred.__call__(2)
        except NotImplementedError:
            assert True, "invoking __call__ on an incomplete subclass of predicate raised the correct error"

        try:
            str(pred)
        except NotImplementedError:
            assert True, "invoking __str__ on an incomplete subclass of predicate raised the correct error"

    def test_Implemented(self):
        pred = predicates.less_than(2)
        try:
            pred.__call__(2)
        except NotImplementedError:
            assert False, "invoking __call__ on an complete subclass of predicate failed"

        try:
            str(pred)
        except NotImplementedError:
            assert False, "invoking __str__ on an complete subclass of predicate failed"
        assert True, "implemented predicate had no problems invoking functions"
    
    def test_is_predicate(self):
        class dummyPred():
            def __call__(self, item):
                pass

            def __str__(self):
                return "dummyPred"

        pred = dummyPred()
        assert predicates.is_predicate(pred), "a class with call and str methods satisfies is_predicate()"
        pred = object()
        assert not predicates.is_predicate(pred), "a class without call and str doesn't satisfy is_predicate()"
        pred = predicates.predicate()
        assert predicates.is_predicate(pred), "an instance of predicate satisfies is_predicate()"
        pred = predicates.equal_to(1)
        assert predicates.is_predicate(pred), "an instance of a subclass of predicate satisfies is_predicate()"


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

    def test_less_than_or_equals(self):
        pred = predicates.less_than_or_equals(1)
        assert pred(1), "one is less than or equal to 1"
        assert pred(-1), "negative one is less than or equal to 1"
        assert not pred(10), "ten is not less than or equal to 1"
        self.check_str(pred)
    
    def test_greater_than_or_equals(self):
        pred = predicates.greater_than_or_equals(1)
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
        pred = predicates.is_a_member_of(['a', 'b', 'c'])
        assert pred('a'), "a is in the set [a, b, c]"
        assert pred('b'), "b is in the set [a, b, c]"
        assert pred('c'), "c is in the set [a, b, c]"
        assert not pred(1), "1 is not in the set [a, b, c]"
        assert not pred('x'), "x is not in the set [a, b, c]"
        self.check_str(pred)
    
    def test_is_not_a_member_of(self):
        pred = predicates.is_not_a_member_of(['a', 'b', 'c'])
        assert pred(1), "1 is not in the set [a, b, c]"
        assert pred('x'), "x is not in the set [a, b, c]"
        assert not pred('a'), "a is not not in the set [a, b, c]"
        assert not pred('b'), "b is not not in the set [a, b, c]"
        assert not pred('c'), "c is not not in the set [a, b, c]"
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
        p_list = []
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
        p_list = []
        p_list.append(predicates.less_than(8))
        p_list.append(predicates.less_than(6))
        p_list.append(predicates.equal_to(4))
        pred = predicates.satisfies_any(p_list)
        assert pred(4), "4 satisfies all predicates in the list"
        assert pred(5), "5 satisfies only 2 predicates in the list"
        assert pred(7), "7 satisfies only 1 predicate in the list"
        assert not pred(9), "9 satisfies only no predicates in the list"
        self.check_str(pred)
    
    def test_args_predicates(self):
        raise NotImplementedError("This predicate test case has yet to be implemented.")

    def test_telemetry_predicates(self):
        raise NotImplementedError("This predicate test case has yet to be implemented.")

    def test_event_predicates(self):
        raise NotImplementedError("This predicate test case has yet to be implemented.")

if __name__ == '__main__':
    unittest.main()
