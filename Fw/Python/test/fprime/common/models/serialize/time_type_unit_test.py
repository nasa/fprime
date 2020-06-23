import os
import sys
import unittest

filename = os.path.dirname(__file__)
fprime_path = os.path.join(filename, "../../../../../src")
sys.path.insert(0, fprime_path)

from fprime.common.models.serialize.time_type import TimeType
from fprime.common.models.serialize.time_type import TimeBase


class TimeTypeTestCases(unittest.TestCase):
    def setUp(self):
        self.t0 = TimeType(0, 0, 0, 0)
        self.t1 = TimeType(0, 0, 1, 0)
        self.t15 = TimeType(0, 0, 1, 500000)

    def test_LessThan(self):
        assert not self.t0 < self.t0, "0 is not less than 0"
        assert not self.t0 < 0, "0 is not less than 0"
        assert self.t0 < self.t1, "0 is less than 1"
        assert self.t0 < 1, "0 is less than 1"
        assert self.t0 < self.t15, "0 is less than 1.5"
        assert self.t0 < 1.5, "0 is less than 1.5"

    def test_LessThanEqualTo(self):
        assert self.t0 <= self.t1, "0 is less than 1"
        assert self.t0 <= 1, "0 is less than 1"
        assert self.t0 <= self.t15, "0 is less than 1.5"
        assert self.t0 <= 1.5, "0 is less than 1.5"
        assert self.t0 <= self.t0, "0 is equal to 0"
        assert self.t0 <= 0, "0 is equal to 0"

    def test_EqualTo(self):
        assert self.t0 == self.t0, "0 is equal to 0"
        assert self.t0 == 0, "0 is equal to 0"
        assert not self.t0 == self.t1, "0 is not equal to 1"
        assert not self.t0 == 1, "0 is not equal to 1"
        assert not self.t0 == self.t15, "0 is not equal to 1.5"
        assert not self.t0 == 1.5, "0 is not equal to 1.5"
        assert self.t15 == 1.5, "1.5 is not equal to 1.5"

    def test_NotEqualTo(self):
        assert not self.t0 != self.t0, "0 is not equal to 0"
        assert not self.t0 != 0, "0 is not equal to 0"
        assert self.t0 != self.t1, "0 is equal to 1"
        assert self.t0 != 1, "0 is equal to 1"
        assert self.t0 != self.t15, "0 is equal to 1.5"
        assert self.t0 != 1.5, "0 is equal to 1.5"

    def test_GreaterThan(self):
        assert self.t15 > self.t0, "1.5 is greater than 0"
        assert self.t15 > 0, "1.5 is greater than 0"
        assert self.t15 > self.t1, "1.5 is greater than 1"
        assert self.t15 > 1, "1.5 is greater than 1"
        assert not self.t15 > self.t15, "1.5 is not greater than 1.5"
        assert not self.t15 > 1.5, "1.5 is not greater than 1.5"

    def test_GreaterThanEqualTo(self):
        assert self.t15 >= self.t0, "1.5 is greater than 0"
        assert self.t15 >= 0, "1.5 is greater than 0"
        assert self.t15 >= self.t1, "1.5 is greater than 1"
        assert self.t15 >= 1, "1.5 is greater than 1"
        assert self.t15 >= self.t15, "1.5 is equal to 1.5"
        assert self.t15 >= 1.5, "1.5 is equal to 1.5"

    def test_Add(self):
        # identity property of addition
        t1_1 = self.t1 + self.t0
        assert t1_1 == self.t1, "1 equals 1"
        assert t1_1 is not self.t1, "the two objects are not the same instance"
        # addition examples
        t5 = self.t1 + self.t1 + self.t1 + 2
        assert t5 == 5, "adding 3 timestamps of 1 and a literal 2 is equal to 5"
        t15_1 = self.t0 + 1.5
        assert t15_1 == self.t15, "adding 1.5 to a timestamps of 0 is equal to 1.5"
        t15_1 = 1.5 + self.t0
        assert t15_1 == self.t15, "adding 1.5 to a timestamps of 0 is equal to 1.5"

    def test_Subtract(self):
        # identity property of subtraction
        t1_1 = self.t1 - self.t0
        assert t1_1 == self.t1, "1 equals 1"
        assert t1_1 is not self.t1, "the two objects are not the same instance"
        # some subtraction examples
        t3 = self.t0 + 3
        t15_1 = t3 - 1.5
        assert self.t15 == t15_1, "subtracting 1.5 from three gives 1.5"
        t27 = t3 - 0.3
        assert t27 == 2.7, "subtracting .3 from 3 equals 2.7"
        t15_1 = 3 - self.t15
        assert self.t15 == t15_1, "subtracting 1.5 from three gives 1.5"

    def test_Multiply(self):
        # identity property of subtraction
        t1_1 = self.t1 * self.t1
        assert t1_1 == self.t1, "1 equals 1"
        assert t1_1 is not self.t1, "the two objects are not the same instance"
        # some multiplication examples
        t5 = self.t1 * 5
        assert t5 == 5, "5 times 1 is 5"
        t6 = self.t15 * 4
        assert t6 == 6, "1.5 times 4 is 6"
        t075 = 0.5 * self.t15
        assert t075 == 0.75, "1.5 times .5 is 0.75"

    def test_TrueDivide(self):
        # identity property of division
        t1_1 = self.t1 / self.t1
        assert t1_1 == self.t1, "1 equals 1"
        assert t1_1 is not self.t1, "the two objects are not the same instance"
        # some division examples
        t6 = self.t0 + 6
        t4 = t6 / 1.5
        assert t4 == 4, "dividing 6 by 1.5 is 4"
        t4 = 6 / self.t15
        assert t4 == 4, "dividing 6 by 1.5 is 4"

    def test_FloorDivide(self):
        # identity property of division
        t1_1 = self.t1 // self.t1
        assert t1_1 == self.t1, "1 equals 1"
        assert t1_1 is not self.t1, "the two objects are not the same instance"
        # some division examples
        t6 = self.t0 + 6
        t3 = t6 // 1.7
        assert t3 == 3, "dividing 6 by 1.7 with floor division is 3"
        t3 = 5.5 // self.t15
        assert t3 == 3, "dividing 5.5 by 1.5 with floor division is 3"


if __name__ == "__main__":
    unittest.main()
