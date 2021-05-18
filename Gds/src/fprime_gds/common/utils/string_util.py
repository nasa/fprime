"""
string_util.py

Utility functions to process strings to be used in FPrime GDS

@Created March 18, 2021
@janamian
"""

import re


def format_string(format_str, values):
    """
    Function to convert C-string style to python format
    without using python interpolation
    Considered the following format for C-string:
    %[flags][width][.precision][length]type
    
    %:           (?<!%)(?:%%)*%
    flags:       ([\-\+0\ \#])?
    width:       (\d+|\*)?
    .precision:  (\.\*|\.\d+)?
    length:      `([hLIw]|l{1,2}|I32|I64)?`
    type:        `([cCdiouxXeEfgGaAnpsSZ])`
    
    Note:
    This function will keep the flags, width, .precision and length of C-string
    template. It will remove all types so they could be duck-typed by python 
    interpreter except for hex type X or x.
    """
    def convert(match_obj):
        if match_obj.group() is not None:
            template = "{:" + match_obj.group()[1:-1]
            if match_obj.group()[-1].lower() == 'x':
                template += match_obj.group()[-1]
            template += "}"
            return template
        return match_obj

    pattern = "(?<!%)(?:%%)*%([\-\+0\ \#])?(\d+|\*)?(\.\*|\.\d+)?([hLIw]|l{1,2}|I32|I64)?([cCdiouxXeEfgGaAnpsSZ])"
    match = re.compile(pattern) 
    formated_str = re.sub(match, convert, format_str)
    result = formated_str.format(*values)
    return result
