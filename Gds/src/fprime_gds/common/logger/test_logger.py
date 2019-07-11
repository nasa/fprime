"""
test_logger.py:

A wrapper on the openpyxl library that provides the ability to log messages in an excel sheet. Used by
the test API to log events, asserts, test cases and user messages.

:author: koran
"""
import time
import datetime
from openpyxl import Workbook
from openpyxl.styles import PatternFill, Font
from openpyxl.cell import WriteOnlyCell


class TestLogger:
    """
    User-accessible colors.
    """

    BROWN = "996633"
    RED = "FF6666"
    ORANGE = "FF9900"
    YELLOW = "FFFF66"
    GREEN = "66FF66"
    BLUE = "6666FF"
    PURPLE = "9933FF"
    GRAY = "C0C0C0"
    WHITE = "FFFFFF"
    """
    User-accessible styles
    """
    BOLD = "BOLD"
    ITALICS = "ITALICS"
    UNDERLINED = "UNDERLINED"

    def __init__(self, filename):
        """
        Constructs a TestLogger

        Args:
            workbook: either a filename or an existing workbook object for the logger to log to.
        """
        if not isinstance(filename, str):
            raise TypeError(
                "Test Logger requires a filename where the output can be saved."
            )
        self.filename = filename
        self.workbook = Workbook(write_only=True)
        self.worksheet = self.workbook.create_sheet()
        header = []
        header.append(self.__get_cell__("Time", style=self.BOLD))
        header.append(self.__get_cell__("Sender", style=self.BOLD))
        header.append(self.__get_cell__("Message", style=self.BOLD))
        self.worksheet.append(header)
    
    def __get_cell__(self, string, color=None, style=None):
        cell = WriteOnlyCell(self.worksheet, value=string)
        if color is not None:
            cell.fill = PatternFill("solid", fgColor=color)
        if style is not None:
            cell.font = Font(
                bold=(style == self.BOLD),
                italic=(style == self.ITALICS),
                underline=("single" if style == self.UNDERLINED else "none"),
            )
        return cell

    def log_message(self, message, sender="NA", color=None, style=None):
        ts = time.time()
        timestring = datetime.datetime.fromtimestamp(ts).strftime("%m/%d/%Y %H:%M:%S")

        row = []
        row.append(self.__get_cell__(timestring, color, style))
        row.append(self.__get_cell__(sender, color, style))
        row.append(self.__get_cell__(message, color, style))
        self.worksheet.append(row)

    def close_log(self):
        self.workbook.save(filename=self.filename)
