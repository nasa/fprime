import os
import sys
import time
import unittest

from fprime.common.models.serialize.numerical_types import I32Type
from fprime.common.models.serialize.time_type import TimeType
from fprime_gds.common.data_types.ch_data import ChData
from fprime_gds.common.history.chrono import ChronologicalHistory
from fprime_gds.common.templates.ch_template import ChTemplate
from fprime_gds.common.testing_fw import predicates

filename = os.path.dirname(__file__)
gdsName = os.path.join(filename, "../../../../src")
fprimeName = os.path.join(filename, "../../../../../Fw/Python/src")
sys.path.insert(0, gdsName)
sys.path.insert(0, fprimeName)


"""
These unit tests were quickly modified from the original tests on the predicate history.
They all run, but may not be written in an optimized way.
"""


class HistoryTestCases(unittest.TestCase):
    def setUp(self):
        self.cHistory = ChronologicalHistory()

    @staticmethod
    def get_range(length):
        temp1 = ChTemplate(1, "Test Channel 1", "Chrono_Hist_Tester", I32Type())
        chList = []
        ts0 = TimeType()
        for item in range(length):
            tsi = ts0 + time.time()
            chList.append(ChData(I32Type(item), tsi, temp1))
        return chList

    @staticmethod
    def assert_lists_equal(expected, actual):
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
        self.assert_lists_equal([], self.cHistory.retrieve())
        tList = []
        for item in self.get_range(100):
            self.cHistory.data_callback(item)
            tList.append(item)
        self.assert_lists_equal(tList, self.cHistory.retrieve())

        self.cHistory.clear()
        rList = list(reversed(tList))
        for item in rList:
            self.cHistory.data_callback(item)
        self.assert_lists_equal(tList, self.cHistory.retrieve())

        self.cHistory.clear()
        self.assert_lists_equal([], self.cHistory.retrieve())

    def test_push_and_retrieve_at_index(self):
        tList = []
        for item in self.get_range(50):
            self.cHistory.data_callback(item)
            tList.append(item)
        for item in self.get_range(50):
            self.cHistory.data_callback(item)
            tList.append(item)
        self.assert_lists_equal(tList[10:], self.cHistory.retrieve(10))

        self.cHistory.clear()
        rList = list(reversed(tList))
        for item in rList:
            self.cHistory.data_callback(item)
        self.assert_lists_equal(tList[10:], self.cHistory.retrieve(10))

    def test_push_and_retrieve_from_predicate(self):
        tList = []
        for item in self.get_range(50):
            self.cHistory.data_callback(item)
            tList.append(item)
        for item in self.get_range(50):
            self.cHistory.data_callback(item)
            tList.append(item)
        start = predicates.equal_to(tList[45])
        self.assert_lists_equal(tList[45:], self.cHistory.retrieve(start))

    def test_push_and_clear(self):
        tList = []
        for item in self.get_range(200):
            self.cHistory.data_callback(item)
            tList.append(item)
        self.assert_lists_equal(tList, self.cHistory.retrieve())
        self.cHistory.clear()
        self.assert_lists_equal([], self.cHistory.retrieve())

    def test_push_and_clear_index(self):
        tList = []
        for item in self.get_range(200):
            self.cHistory.data_callback(item)
            tList.append(item)
        self.assert_lists_equal(tList, self.cHistory.retrieve())
        self.cHistory.clear(100)
        self.assert_lists_equal(tList[100:], self.cHistory.retrieve())

    def test_push_and_clear_predicate(self):
        tList = []
        for item in self.get_range(50):
            self.cHistory.data_callback(item)
            tList.append(item)
        for item in self.get_range(50):
            self.cHistory.data_callback(item)
            tList.append(item)
        self.assert_lists_equal(tList, self.cHistory.retrieve())
        start = predicates.equal_to(tList[45])
        self.cHistory.clear(start)
        self.assert_lists_equal(tList[45:], self.cHistory.retrieve())

    def test_history_size(self):
        assert self.cHistory.size() == 0, "starting history is empty"
        for item in self.get_range(50):
            self.cHistory.data_callback(item)
        assert self.cHistory.size() == 50, "starting history is empty"
        self.cHistory.clear(25)
        assert self.cHistory.size() == 25, "starting history is empty"

    def test_history_retrieve_new(self):
        tList = []
        for item in self.get_range(50):
            self.cHistory.data_callback(item)
            tList.append(item)

        self.assert_lists_equal(tList, self.cHistory.retrieve())
        tList.clear()

        for item in self.get_range(30):
            self.cHistory.data_callback(item)
            tList.append(item)

        self.assert_lists_equal(tList, self.cHistory.retrieve_new())
        tList.clear()

        for item in self.get_range(50):
            self.cHistory.data_callback(item)
            tList.append(item)

        self.assert_lists_equal(tList, self.cHistory.retrieve_new())
        self.assert_lists_equal([], self.cHistory.retrieve_new())

    def test_history_retrieve_new_repeat(self):
        tList = self.get_range(50)
        cHistory2 = ChronologicalHistory()
        for item in tList[1:]:
            self.cHistory.data_callback(item)
            cHistory2.data_callback(item)

        self.assert_lists_equal(tList[1:], self.cHistory.retrieve_new(True))
        self.assert_lists_equal(tList[1:], cHistory2.retrieve_new(False))

        self.cHistory.data_callback(tList[0])
        cHistory2.data_callback(tList[0])

        self.assert_lists_equal(tList, self.cHistory.retrieve_new(True))
        self.assert_lists_equal([tList[0]], cHistory2.retrieve_new(False))

    def test_history_retrieve_new_after_clear(self):
        tList = []
        for item in self.get_range(50):
            self.cHistory.data_callback(item)
            tList.append(item)

        self.assert_lists_equal(tList, self.cHistory.retrieve())
        tList.clear()

        for item in self.get_range(50):
            self.cHistory.data_callback(item)
            tList.append(item)

        self.cHistory.clear(75)

        self.assert_lists_equal(tList[25:], self.cHistory.retrieve_new())
        tList.clear()

    def test_history_iterable(self):
        for item in self.get_range(50):
            self.cHistory.data_callback(item)
        tList = []
        for item in self.cHistory:
            tList.append(item)
        self.assert_lists_equal(tList, self.cHistory.retrieve_new())

    def test_history_length(self):
        assert len(self.cHistory) == 0, "starting history is empty"
        for item in self.get_range(50):
            self.cHistory.data_callback(item)
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
        self.cHistory = ChronologicalHistory(pred)
        for item in self.get_range(50):
            self.cHistory.data_callback(item)

        assert len(self.cHistory) == 25

        correct_error = False
        try:
            ChronologicalHistory("Not a predicate")
        except TypeError:
            correct_error = True
        assert correct_error, "The History should have raised a TypeError"


if __name__ == "__main__":
    unittest.main()
