"""
Created on Dec 18, 2014

@author: tcanham

"""
from __future__ import print_function
from __future__ import absolute_import

# Exception classes for all types


class TypeException(Exception):
    def __init__(self, val):
        self.except_msg = val

    def getMsg(self):
        return self.except_msg


class AbstractMethodException(TypeException):
    def __init__(self, val):
        super(AbstractMethodException, self).__init__(
            "%s must be implemented since it is abstract!" % str(val)
        )


class TypeRangeException(TypeException):
    def __init__(self, val):
        super(TypeRangeException, self).__init__("Value %s out of range!" % str(val))


class StringSizeException(TypeException):
    def __init__(self, size, max_size):
        super(StringSizeException, self).__init__(
            "String size %s is greater than %s!" % (str(size), str(max_size))
        )


class TypeMismatchException(TypeException):
    def __init__(self, expected_type, actual_type):
        super(TypeMismatchException, self).__init__(
            "Type %s expected, type %s found!" % (expected_type, actual_type)
        )


class ArrayLengthException(TypeException):
    def __init__(self, arr_type, expected_len, actual_len):
        super(ArrayLengthException, self).__init__(
            "Array type %s is of length %s, actual length %s found!"
            % (arr_type, expected_len, actual_len)
        )


class EnumMismatchException(TypeException):
    def __init__(self, enum, bad_member):
        super(EnumMismatchException, self).__init__(
            "Invalid enum member %s set in %s enum!" % (bad_member, enum)
        )


class DeserializeException(TypeException):
    def __init__(self, message):
        super(DeserializeException, self).__init__(message)


class ArgNotFoundException(TypeException):
    def __init__(self, message):
        super(ArgNotFoundException, self).__init__("Arg %s not found!" % message)


class NotInitializedException(TypeException):
    def __init__(self, message):
        super(NotInitializedException, self).__init__(
            "Instance %s not initialized!" % message
        )


class NotOverridenException(TypeException):
    def __init__(self, message):
        super(NotOverridenException, self).__init__(
            "Required base class method not overrwritten in type %s!" % message
        )


class ArgLengthMismatchException(TypeException):
    def __init__(self, arg_length_actual, arg_length_given):
        super(ArgLengthMismatchException, self).__init__(
            "%d args provided, but command expects %d args!"
            % (arg_length_given, arg_length_actual)
        )


class CompoundTypeLengthMismatchException(TypeException):
    def __init__(self, field_length_actual, field_length_given):
        super(CompoundTypeLengthMismatchException, self).__init__(
            "%d fields provided, but compound type expects %d fields!"
            % (field_length_given, field_length_actual)
        )


if __name__ == "__main__":

    print("TypeException")
    try:
        raise TypeException("Test TypeException")
    except TypeException as e:
        print("Exception: %s" % e.getMsg())

    print("TypeRangeException")
    try:
        raise TypeRangeException("Test Value")
    except TypeRangeException as e:
        print("Exception: %s" % e.getMsg())

    print("TypeMismatchException")
    try:
        raise TypeMismatchException("EXPECTED_TYPE", "ACTUAL_TYPE")
    except TypeMismatchException as e:
        print("Exception: %s" % e.getMsg())

    print("EnumMismatchException")
    try:
        raise EnumMismatchException("BAD_MEMBER", "ENUM")
    except EnumMismatchException as e:
        print("Exception: %s" % e.getMsg())

    print("DeserializeException")
    try:
        raise DeserializeException("MESSAGE")
    except DeserializeException as e:
        print("Exception: %s" % e.getMsg())

    print("ArgNotFoundException")
    try:
        raise ArgNotFoundException("MESSAGE")
    except ArgNotFoundException as e:
        print("Exception: %s" % e.getMsg())
