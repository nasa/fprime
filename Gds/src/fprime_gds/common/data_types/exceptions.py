"""
Created on Jan 9, 2015
@author: reder
"""
# Exception classes for all controllers modules


class GseControllerException(Exception):
    def __init__(self, val):
        self.except_msg = val
        super().__init__(val)

    def getMsg(self):
        return self.except_msg


class GseControllerUndefinedDirectoryException(GseControllerException):
    def __init__(self, val):
        super().__init__(f"Path does not exist: {str(val)}!")


class GseControllerUndefinedFileException(GseControllerException):
    def __init__(self, val):
        super().__init__(f"Path does not exist: {str(val)}!")


class GseControllerParsingException(GseControllerException):
    def __init__(self, val):
        super().__init__(f"Parsing error: {str(val)}")


class GseControllerMnemonicMismatchException(GseControllerException):
    def __init__(self, val1, val2):
        super().__init__(f"ID mismatch ({str(val1)}, {str(val2)})!")


class GseControllerStatusUpdateException(GseControllerException):
    def __init__(self, val):
        super().__init__(f"Bad status update mode: {str(val)}!")
