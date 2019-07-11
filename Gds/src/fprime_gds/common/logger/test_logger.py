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
from openpyxl.styles import PatternFill, Font, Alignment
from openpyxl.cell import WriteOnlyCell


class TestLogger:
    """
    User-accessible colors. Can be used for the color arguments
    """
    BROWN = "E6CCB3"
    RED = "FF9999"
    ORANGE = "FFCC99"
    YELLOW = "FFFF99"
    GREEN = "ADEBAD"
    BLUE = "99CCFF"
    PURPLE = "CC99FF"
    GRAY = "D9D9D9"
    WHITE = "FFFFFF"

    """
    User-accessible styles. Must be used for the style arguments
    """
    BOLD = "BOLD"
    ITALICS = "ITALICS"
    UNDERLINED = "UNDERLINED"

    __align = Alignment(vertical='top', wrap_text=True)
    __font_name = 'courier'
    __time_fmt = "%H:%M:%S.%f"#"%m/%d/%Y %H:%M:%S.%f"

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

        ts = time.time()
        timestring = datetime.datetime.fromtimestamp(ts).strftime(self.__time_fmt)
        self.worksheet.column_dimensions['A'].width = len(timestring) + 1
        self.worksheet.column_dimensions['C'].width = 120

        header = []
        header.append(self.__get_cell("Time", style=self.BOLD))
        header.append(self.__get_cell("Sender", style=self.BOLD))
        header.append(self.__get_cell("Message", style=self.BOLD))
        self.worksheet.append(header)

    def __get_cell(self, string, color=None, style=None):
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
        cell.font = Font(
            name=self.__font_name,
            bold=(style == self.BOLD),
            italic=(style == self.ITALICS),
            underline=("single" if style == self.UNDERLINED else "none"),
        )
            
        cell.alignment = self.__align
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
        timestring = datetime.datetime.fromtimestamp(ts).strftime(self.__time_fmt)

        row = []
        row.append(self.__get_cell(timestring, color, style))
        row.append(self.__get_cell(sender, color, style))
        row.append(self.__get_cell(message, color, style))
        self.worksheet.append(row)

    def close_log(self):
        """
        Saves the write-only workbook. Should be called only once when the log is completed.
        """
        self.workbook.save(filename=self.filename)
