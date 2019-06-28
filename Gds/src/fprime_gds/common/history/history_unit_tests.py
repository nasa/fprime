import unittest
import sys
import os
filename = os.path.dirname(__file__)
gdsName = os.path.join(filename, '../../../')
fprimeName = os.path.join(filename, '../../../../../Fw/Python/src')
sys.path.insert(0, gdsName)
sys.path.insert(0, fprimeName)

from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.history.test import TestHistory


class HistoryTestCases(unittest.TestCase):
    def setUp(self):
        self.tHistory = TestHistory()

    def assert_lists_equal(self, listA, listB):
        assert len(listA) == len(listB), "assert that both lists to compare are of equal length({}, {})".format(len(listA), len(listB))
        for i in range(len(listA)):
            assert listA[i] == listB[i], "assert that element {} of the lists are equal ({} == {})".format(i, listA[i], listB[i])

    def test_push_and_retrieve(self):
        tList = []
        for i in range(200):
            self.tHistory.data_callback(i)
            tList.append(i)
        self.assert_lists_equal(tList, self.tHistory.retrieve())

    def test_push_and_retrieve_at_index(self):
        tList = []
        for i in range(50):
            self.tHistory.data_callback(i)
            tList.append(i)
        self.tHistory.data_callback("hello")
        tList.append("hello")
        for i in range(50):
            self.tHistory.data_callback(i)
            tList.append(i)
        self.assert_lists_equal(tList[10:], self.tHistory.retrieve(10))

    def test_push_and_retrieve_from_predicate(self):
        tList = []
        for i in range(50):
            self.tHistory.data_callback(i)
            tList.append(i)
        self.tHistory.data_callback("hello")
        tList.append("hello")
        for i in range(50):
            self.tHistory.data_callback(i)
            tList.append(i)
        start = predicates.equal_to("hello")
        self.assert_lists_equal(tList[50:], self.tHistory.retrieve(start))

    def test_push_and_clear(self):
        tList = []
        for i in range(200):
            self.tHistory.data_callback(i)
            tList.append(i)
        self.assert_lists_equal(tList, self.tHistory.retrieve())
        self.tHistory.clear()
        self.assert_lists_equal([], self.tHistory.retrieve())

    def test_push_and_clear_index(self):
        tList = []
        for i in range(200):
            self.tHistory.data_callback(i)
            tList.append(i)
        self.assert_lists_equal(tList, self.tHistory.retrieve())
        self.tHistory.clear(100)
        self.assert_lists_equal(tList[100:], self.tHistory.retrieve())
    
    def test_push_and_clear_predicate(self):
        tList = []
        for i in range(50):
            self.tHistory.data_callback(i)
            tList.append(i)
        self.tHistory.data_callback("hello")
        tList.append("hello")
        for i in range(50):
            self.tHistory.data_callback(i)
            tList.append(i)
        self.assert_lists_equal(tList, self.tHistory.retrieve())
        start = predicates.equal_to("hello")
        self.tHistory.clear(start)
        self.assert_lists_equal(tList[50:], self.tHistory.retrieve())

    def test_history_size(self):
        assert self.tHistory.size() == 0, "starting history is empty"
        for i in range(50):
            self.tHistory.data_callback(i)
        assert self.tHistory.size() == 50, "starting history is empty"
        self.tHistory.clear(25)
        assert self.tHistory.size() == 25, "starting history is empty"
    
    def test_history_retrieve_new(self):
        tList = []
        for i in range(50):
            self.tHistory.data_callback(i)
            tList.append(i)

        self.assert_lists_equal(tList, self.tHistory.retrieve())
        tList.clear()

        self.tHistory.data_callback("hello")
        tList.append("hello")

        self.assert_lists_equal(tList, self.tHistory.retrieve_new())
        tList.clear()

        for i in range(50):
            self.tHistory.data_callback(i)
            tList.append(i)

        self.assert_lists_equal(tList, self.tHistory.retrieve_new())

    def test_history_retrieve_new_after_clear(self):
        tList = []
        for i in range(50):
            self.tHistory.data_callback(i)
            tList.append(i)

        self.assert_lists_equal(tList, self.tHistory.retrieve())
        tList.clear()

        for i in range(50):
            self.tHistory.data_callback(i)
            tList.append(i)

        self.tHistory.clear(75)

        self.assert_lists_equal(tList[25:], self.tHistory.retrieve_new())
        tList.clear()

    def test_history_iterable(self):
        for i in range(50):
            self.tHistory.data_callback(i)
        tList = []
        for item in self.tHistory:
            tList.append(item)
        self.assert_lists_equal(tList, self.tHistory.retrieve_new())

    def test_history_length(self):
        assert len(self.tHistory) == 0, "starting history is empty"
        for i in range(50):
            self.tHistory.data_callback(i)
        assert len(self.tHistory) == 50, "starting history is empty"
        self.tHistory.clear(25)
        assert len(self.tHistory) == 25, "starting history is empty"

    def test_history_filter(self):
        class is_even(predicates.predicate):
            def __call__(self, item):
                return item % 2 == 0
            def __str__(self):
                return "Decides if a value is even"

        pred = is_even()
        self.tHistory = TestHistory(filter_pred=pred)
        for i in range(50):
            self.tHistory.data_callback(i)
        assert len(self.tHistory) == 25
        self.assert_lists_equal(range(0, 50, 2), self.tHistory.retrieve())


if __name__ == '__main__':
    unittest.main()
