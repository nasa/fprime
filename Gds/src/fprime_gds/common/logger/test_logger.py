"""
test_logger.py:

A wrapper on the openpyxl library that provides the ability to log messages in an excel sheet. Used
by the test API to log events, asserts, test cases and user messages. The documentation for openpyxl
can be found here:
https://openpyxl.readthedocs.io/en/stable/index.html

This class uses a write-only optimization that should allow for creating large log files without
hogging too much memory. Write-only optimization can be found here:
https://openpyxl.readthedocs.io/en/stable/optimized.html#write-only-mode

:author: koran
"""
import time
import datetime
from openpyxl import Workbook
from openpyxl.styles import PatternFill, Font
from openpyxl.cell import WriteOnlyCell


class TestLogger:
    """
    User-accessible colors. Can be used for the color arguments
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
    User-accessible styles. Must be used for the style arguments
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
        """
        Helper method for log message. This method takes a string as well as color and style
        arguments to create a write-only cell.

        Args:
            string: a string object to be written to the cell's value field.
            color: a string object containing a color hex code "######"
            style: a string choosing 1 of 3 formatting options (ITALICS, BOLD, UNDERLINED)
        """
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
        """
        Logs a message to the TestLog. Each message will include a timestamp beforehand.

        Args:
            message: a message to log (str).
            sender: a short string describing who created the log message
            color: a string object containing a color hex code "######"
            style: a string choosing 1 of 3 formatting options (ITALICS, BOLD, UNDERLINED)
        """
        ts = time.time()
        timestring = datetime.datetime.fromtimestamp(ts).strftime("%m/%d/%Y %H:%M:%S")

        row = []
        row.append(self.__get_cell__(timestring, color, style))
        row.append(self.__get_cell__(sender, color, style))
        row.append(self.__get_cell__(message, color, style))
        self.worksheet.append(row)

    def close_log(self):
        """
        Saves the write-only workbook. Should be called only once when the log is completed.
        """
        self.workbook.save(filename=self.filename)
