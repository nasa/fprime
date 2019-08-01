import os
import sys
import time
import unittest

filename = os.path.dirname(__file__)
gdsName = os.path.join(filename, "../../../../src")
fprimeName = os.path.join(filename, "../../../../../Fw/Python/src")
sys.path.insert(0, gdsName)
sys.path.insert(0, fprimeName)

from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.history.chrono import ChronologicalHistory
from fprime.common.models.serialize.time_type import TimeType
from fprime.common.models.serialize.i32_type import I32Type

from fprime_gds.common.templates.ch_template import ChTemplate
from fprime_gds.common.data_types.ch_data import ChData


class HistoryTestCases(unittest.TestCase):
    def setUp(self):
        self.cHistory = ChronologicalHistory()

    def get_range(self, length):
        temp1 = ChTemplate(1, "Test Channel 1", "Chrono_Hist_Tester", I32Type())
        chList = []
        ts0 = TimeType()
        for i in range(length):
            tsi = ts0 + time.time()
            chList.append(ChData(I32Type(i), tsi, temp1))
        return chList

    def assert_lists_equal(self, expected, actual):
        assert len(expected) == len(
            actual
        ), "the given list should have had the length {}, but instead had {}".format(
            len(expected), len(actual)
        )
        for i in range(len(expected)):
            assert (
                expected[i] is actual[i]
            ), "the {} element of the expected list should be {}, but was {}.".format(
                i, expected[i], actual[i]
            )

    def test_push_and_retrieve(self):
        tList = []
        for i in self.get_range(200):
            self.cHistory.data_callback(i)
            tList.append(i)
        self.assert_lists_equal(tList, self.cHistory.retrieve())

    def test_push_and_retrieve_at_index(self):
        tList = []
        for i in self.get_range(50):
            self.cHistory.data_callback(i)
            tList.append(i)
        for i in self.get_range(50):
            self.cHistory.data_callback(i)
            tList.append(i)
        self.assert_lists_equal(tList[10:], self.cHistory.retrieve(10))

    def test_push_and_retrieve_from_predicate(self):
        tList = []
        for i in self.get_range(50):
            self.cHistory.data_callback(i)
            tList.append(i)
        for i in self.get_range(50):
            self.cHistory.data_callback(i)
            tList.append(i)
        start = predicates.equal_to(tList[45])
        self.assert_lists_equal(tList[45:], self.cHistory.retrieve(start))

    def test_push_and_clear(self):
        tList = []
        for i in self.get_range(200):
            self.cHistory.data_callback(i)
            tList.append(i)
        self.assert_lists_equal(tList, self.cHistory.retrieve())
        self.cHistory.clear()
        self.assert_lists_equal([], self.cHistory.retrieve())

    def test_push_and_clear_index(self):
        tList = []
        for i in self.get_range(200):
            self.cHistory.data_callback(i)
            tList.append(i)
        self.assert_lists_equal(tList, self.cHistory.retrieve())
        self.cHistory.clear(100)
        self.assert_lists_equal(tList[100:], self.cHistory.retrieve())

    def test_push_and_clear_predicate(self):
        tList = []
        for i in self.get_range(50):
            self.cHistory.data_callback(i)
            tList.append(i)
        for i in self.get_range(50):
            self.cHistory.data_callback(i)
            tList.append(i)
        self.assert_lists_equal(tList, self.cHistory.retrieve())
        start = predicates.equal_to(tList[45])
        self.cHistory.clear(start)
        self.assert_lists_equal(tList[45:], self.cHistory.retrieve())

    def test_history_size(self):
        assert self.cHistory.size() == 0, "starting history is empty"
        for i in self.get_range(50):
            self.cHistory.data_callback(i)
        assert self.cHistory.size() == 50, "starting history is empty"
        self.cHistory.clear(25)
        assert self.cHistory.size() == 25, "starting history is empty"

    def test_history_retrieve_new(self):
        tList = []
        for i in self.get_range(50):
            self.cHistory.data_callback(i)
            tList.append(i)

        self.assert_lists_equal(tList, self.cHistory.retrieve())
        tList.clear()

        for i in self.get_range(30):
            self.cHistory.data_callback(i)
            tList.append(i)

        self.assert_lists_equal(tList, self.cHistory.retrieve_new())
        tList.clear()

        for i in self.get_range(50):
            self.cHistory.data_callback(i)
            tList.append(i)

        self.assert_lists_equal(tList, self.cHistory.retrieve_new())

    def test_history_retrieve_new_after_clear(self):
        tList = []
        for i in self.get_range(50):
            self.cHistory.data_callback(i)
            tList.append(i)

        self.assert_lists_equal(tList, self.cHistory.retrieve())
        tList.clear()

        for i in self.get_range(50):
            self.cHistory.data_callback(i)
            tList.append(i)

        self.cHistory.clear(75)

        self.assert_lists_equal(tList[25:], self.cHistory.retrieve_new())
        tList.clear()

    def test_history_iterable(self):
        for i in self.get_range(50):
            self.cHistory.data_callback(i)
        tList = []
        for item in self.cHistory:
            tList.append(item)
        self.assert_lists_equal(tList, self.cHistory.retrieve_new())

    def test_history_length(self):
        assert len(self.cHistory) == 0, "starting history is empty"
        for i in self.get_range(50):
            self.cHistory.data_callback(i)
        assert len(self.cHistory) == 50, "starting history is empty"
        self.cHistory.clear(25)
        assert len(self.cHistory) == 25, "starting history is empty"

    def test_history_filter(self):
        class is_even(predicates.predicate):
            def __call__(self, item):
                return item.get_val() % 2 == 0

            def __str__(self):
                return "Decides if a value is even"

        pred = is_even()
        self.cHistory = ChronologicalHistory(filter_pred=pred)
        for i in self.get_range(50):
            self.cHistory.data_callback(i)

        assert len(self.cHistory) == 25


if __name__ == "__main__":
    unittest.main()
