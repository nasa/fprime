"""
Created on Dec 18, 2014

@author: tcanham

"""

# Exception classes for all types


class TypeException(Exception):
    def __init__(self, val):
        super().__init__(val)
        self.except_msg = val

    def getMsg(self):
        return self.except_msg


class AbstractMethodException(TypeException):
    def __init__(self, val):
        super().__init__("%s must be implemented since it is abstract!" % str(val))


class TypeRangeException(TypeException):
    def __init__(self, val):
        super().__init__("Value %s out of range!" % str(val))


class StringSizeException(TypeException):
    def __init__(self, size, max_size):
        super().__init__(
            "String size {} is greater than {}!".format(str(size), str(max_size))
        )


class TypeMismatchException(TypeException):
    def __init__(self, expected_type, actual_type):
        super().__init__(
            "Type {} expected, type {} found!".format(expected_type, actual_type)
        )


class ArrayLengthException(TypeException):
    def __init__(self, arr_type, expected_len, actual_len):
        super().__init__(
            "Array type %s is of length %s, actual length %s found!"
            % (arr_type, expected_len, actual_len)
        )


class EnumMismatchException(TypeException):
    def __init__(self, enum, bad_member):
        super().__init__(
            "Invalid enum member {} set in {} enum!".format(bad_member, enum)
        )


class DeserializeException(TypeException):
    pass


class ArgNotFoundException(TypeException):
    def __init__(self, message):
        super().__init__("Arg %s not found!" % message)


class NotInitializedException(TypeException):
    def __init__(self, message):
        super().__init__("Instance %s not initialized!" % message)


class NotOverridenException(TypeException):
    def __init__(self, message):
        super().__init__(
            "Required base class method not overrwritten in type %s!" % message
        )


class ArgLengthMismatchException(TypeException):
    def __init__(self, arg_length_actual, arg_length_given):
        super().__init__(
            "%d args provided, but command expects %d args!"
            % (arg_length_given, arg_length_actual)
        )


class CompoundTypeLengthMismatchException(TypeException):
    def __init__(self, field_length_actual, field_length_given):
        super().__init__(
            "%d fields provided, but compound type expects %d fields!"
            % (field_length_given, field_length_actual)
        )
