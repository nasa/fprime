# Test Logger

The Test API produces a formatted test log using the TestLogger

test_logger.py:

A wrapper on the openpyxl library that provides the ability to log messages in an excel sheet. Used
by the test API to log events, asserts, test cases, and user messages. The documentation for openpyxl
can be found here:
[https://openpyxl.readthedocs.io/en/stable/index.html](https://openpyxl.readthedocs.io/en/stable/index.html)

This class uses a write-only optimization that should allow for creating large log files without
hogging too much memory. The write-only optimization can be found here:
[https://openpyxl.readthedocs.io/en/stable/optimized.html#write-only-mode](https://openpyxl.readthedocs.io/en/stable/optimized.html#write-only-mode)


* **author**

    koran



#### class fprime_gds.common.logger.test_logger.TestLogger(output_path, time_format=None, font_name=None)
Bases: `object`

User-accessible colors. Can be used for the color arguments


#### WHITE( = 'FFFFFF')
User-accessible styles. Must be used for the style arguments


#### __init__(output_path, time_format=None, font_name=None)
Constructs a TestLogger

Args:

    output_path: a path where log files will
    time_format: an optional string to specify the timestamp format. See datetime.strftime
    font_name: an optional string to specify the font


#### log_message(message, sender='NA', color=None, style=None, case_id=None)
Logs a message to the TestLog. Each message will include a timestamp beforehand.
Note: Once specified, the test case’s case_id will persist in the logs until it is
specified again.

Args:

    message: a message to log (str).
    sender: a short string describing who created the log message
    color: a string object containing a color hex code “######”
    style: a string choosing 1 of 3 formatting options (ITALICS, BOLD, UNDERLINED)
    case_id: a short identifier to denote which test case the log message belongs to


#### _TestLogger__get_cell(string, color=None, style=None)
Helper method for log message. This method takes a string as well as color and style
arguments to create a write-only cell.

Args:

    string: a string object to be written to the cell’s value field.
    color: a string object containing a color hex code “######”
    style: a string choosing 1 of 3 formatting options (ITALICS, BOLD, UNDERLINED)

#### close_log()
Saves the write-only workbook. Should be called only once when the log is completed.

|Quick Links|
|:----------|
|[Integration Test API User Guide](../user_guide.md)|
|[GDS Overview](../../../../../Gds/README.md)|
|[Integration Test API](integration_test_api.md)|
|[Histories](histories.md)|
|[Predicates](predicates.md)|
|[Test Logger](test_logger.md)|
|[Standard Pipeline](standard_pipeline.md)|
|[TimeType Serializable](time_type.md)|