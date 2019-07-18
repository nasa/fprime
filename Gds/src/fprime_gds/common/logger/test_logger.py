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
    __font_name = 'calibri'
    __time_fmt = "%H:%M:%S.%f"

    def __init__(self, filename, time_format=None, font_name=None):
        """
        Constructs a TestLogger

        Args:
            filename: a filename for the logger to log to.
            time_format: an optional string to specify how the timestamp should be formatted. See datetime.strftime
            font_name: an optional string to specify the font
        """
        if not isinstance(filename, str):
            raise TypeError(
                "Test Logger requires a filename where the output can be saved."
            )
        self.filename = filename
        self.workbook = Workbook(write_only=True)
        self.worksheet = self.workbook.create_sheet()

        if time_format is None:
            self.time_format = self.__time_fmt
        else:
            self.time_format = time_format

        if font_name is None:
            self.font_name = self.__font_name
        else:
            self.font_name = font_name


        ts = time.time()
        timestring = datetime.datetime.fromtimestamp(ts).strftime(self.time_format)
        self.worksheet.column_dimensions['A'].width = len(timestring) + 1
        self.worksheet.column_dimensions['D'].width = 120

        top = []
        date_string = datetime.datetime.fromtimestamp(ts).strftime("%H:%M:%S.%f on %m/%d/%Y")
        top.append(self.__get_cell("Test began at " + date_string, style=self.BOLD))
        self.worksheet.append(top)

        header = []
        header.append(self.__get_cell("Time", style=self.BOLD))
        header.append(self.__get_cell("Case ID", style=self.BOLD))
        header.append(self.__get_cell("Sender", style=self.BOLD))
        header.append(self.__get_cell("Message", style=self.BOLD))
        self.worksheet.append(header)

        self.case_id = "NA"

    def log_message(self, message, sender="NA", color=None, style=None, case_id=None):
        """
        Logs a message to the TestLog. Each message will include a timestamp beforehand.
        Note: Once specified, the test case's case_id will persist in the logs until it is
        specified again.

        Args:
            message: a message to log (str).
            sender: a short string describing who created the log message
            color: a string object containing a color hex code "######"
            style: a string choosing 1 of 3 formatting options (ITALICS, BOLD, UNDERLINED)
            case_id: a short identifier to denote which test case the log message belongs to
        """
        ts = time.time()
        timestring = datetime.datetime.fromtimestamp(ts).strftime(self.time_format)

        if case_id is not None:
            if not isinstance(case_id, str):
                case_id = str(case_id)
            self.case_id = case_id

        row = []
        row.append(self.__get_cell(timestring, color, style))
        row.append(self.__get_cell(self.case_id, color, style))
        row.append(self.__get_cell(sender, color, style))
        row.append(self.__get_cell(message, color, style))
        self.worksheet.append(row)

    def close_log(self):
        """
        Saves the write-only workbook. Should be called only once when the log is completed.
        """
        self.workbook.save(filename=self.filename)

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
            name=self.font_name,
            bold=(style == self.BOLD),
            italic=(style == self.ITALICS),
            underline=("single" if style == self.UNDERLINED else "none"),
        )
        cell.alignment = self.__align
        return cell