"""
Tests format_string util

@Created on March 18, 2021
@janamian
"""

import unittest
from fprime_gds.common.utils.string_util import format_string


class TestFormatString(unittest.TestCase):

    def test_format_with_no_issue(self):
        template = 'Opcode 0x%04X dispatched to port %d and value %f'
        values = (181, 8, 1.234)
        expected = 'Opcode 0x00B5 dispatched to port 8 and value 1.234'
        actual = format_string(template, values)
        self.assertEqual(expected, actual)

    def test_format_value_with_string_input_as_other_types(self):
        template = 'Opcode 0x%04X dispatched to port %d and value %f'
        values = (181, "8", "1.234")
        expected = 'Opcode 0x00B5 dispatched to port 8 and value 1.234'
        actual = format_string(template, values)
        self.assertEqual(expected, actual)

    def test_format_with_format_spec(self):
        template = 'Opcode 0x%04X dispatched to port %04d and value %0.02f'
        values = (181, 8, 1.234)
        expected = 'Opcode 0x00B5 dispatched to port 0008 and value 1.2'
        actual = format_string(template, values)
        self.assertEqual(expected, actual)

    def test_format_bad_case(self):
        template = 'Opcode 0x%04X dispatched with value %f'
        values = ("181", "0.123")
        with self.assertRaises(ValueError):
            self.assertEqual(format_string(template, values))


if __name__ == '__main__':
    unittest.main()
