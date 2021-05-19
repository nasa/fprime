"""
exceptions.py:

Exceptions defined for the Autocoder. This is active refactoring work, and may be somewhat incomplete.

@author mstarch
"""


class FprimeException(Exception):
    """ Base Exception for FprimeException """


class FprimeRngXmlValidationException(FprimeException):
    """ XML Exception from within the validation phase """

    def __init__(self, error_list):
        """
        Construct the exception from a user supplied message and a set of validation errors.

        :param error_list: error list set supplied from validator
        """
        last = str(error_list)
        last = last.replace("<string>:0:0:", "")
        super().__init__("Errors detected in XML:\n{}".format(last))


class FprimeXmlException(FprimeException):
    """ Generic XML error """
