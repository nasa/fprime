"""
Tests the serialization and deserialization of all the types in Fw/Python/src/fprime/common/models/serialize/

Created on Jun 25, 2020
@author: hpaulson, mstarch
"""
import json
from collections.abc import Iterable

import pytest

from fprime.common.models.serialize.array_type import ArrayType
from fprime.common.models.serialize.bool_type import BoolType
from fprime.common.models.serialize.enum_type import EnumType
from fprime.common.models.serialize.numerical_types import (
    F32Type,
    F64Type,
    I8Type,
    I16Type,
    I32Type,
    I64Type,
    U8Type,
    U16Type,
    U32Type,
    U64Type,
)
from fprime.common.models.serialize.serializable_type import SerializableType
from fprime.common.models.serialize.string_type import StringType
from fprime.common.models.serialize.time_type import TimeBase, TimeType

from fprime.common.models.serialize.type_exceptions import (
    DeserializeException,
    NotInitializedException,
    TypeMismatchException,
    TypeRangeException,
)

PYTHON_TESTABLE_TYPES = [
    True,
    False,
    -1,
    0,
    300,
    "abc",
    "True",
    "False",
    3.1412,
    (0, 1),
    (True, False),
    [],
    [0],
    {},
    {"abc": 123},
    {2, 4, 3},
]


def valid_values_test(type_input, valid_values, sizes, extras=None):
    """ Tests to be run on all types """
    if not isinstance(sizes, Iterable):
        sizes = [sizes] * len(valid_values)
    # Should be able to instantiate a blank type, but not serialize it until a value has been supplied
    if not extras:
        extras = [[]] * len(valid_values)
    instantiation = type_input(*extras[0])
    with pytest.raises(NotInitializedException):
        instantiation.serialize()
    # Should be able to get a JSONable object that is dumpable to a JSON string
    jsonable = instantiation.to_jsonable()
    json.loads(json.dumps(jsonable))

    # Run on valid values
    for value, size, extra in zip(valid_values, sizes, extras):
        instantiation = type_input(*extra, val=value)
        assert instantiation.val == value
        assert instantiation.getSize() == size

        # Check assignment by value
        by_value = type_input(*extra)
        by_value.val = value
        assert by_value.val == instantiation.val, "Assignment by value has failed"
        assert by_value.getSize() == size

        # Check serialization and deserialization
        serialized = instantiation.serialize()
        for offset in [0, 10, 50]:
            deserializer = type_input(*extra)
            deserializer.deserialize((b" " * offset) + serialized, offset)
            assert instantiation.val == deserializer.val, "Deserialization has failed"
            assert deserializer.getSize() == size


def invalid_values_test(
    type_input, invalid_values, exception_class=TypeMismatchException
):
    """ Check invalid values for all types """
    for item in invalid_values:
        # Constructor initialization
        with pytest.raises(exception_class):
            instantiation = type_input(item)
        # Value initialization
        with pytest.raises(exception_class):
            instantiation = type_input()
            instantiation.val = item
        # Deserialization problems required space
        for offset in [0, 10, 50]:
            with pytest.raises(DeserializeException):
                instantiation = type_input()
                instantiation.deserialize(b" " * offset, offset)


def ser_deser_time_test(t_base, t_context, secs, usecs):
    """
    Test serialization/deserialization of TimeType objects.

    This test function creates a time type object with the given parameters and
    then serializes it and deserializes it. Also prints it for visual inspection
    of the formatted output.

    Args:
        t_base (int): Time base for the new time type object
        t_context (int): Time context for the new time type object
        secs (int): Seconds value for the new time type object
        usecs (int): Seconds value for the new time type object
        should_err (int): True if error expected, else False

    Returns:
        True if test passed, False otherwise
    """
    val = TimeType(t_base, t_context, secs, usecs)

    buff = val.serialize()

    val2 = TimeType()
    val2.deserialize(buff, 0)

    assert val2.timeBase.value == t_base
    assert val2.timeContext == t_context
    assert val2.seconds == secs
    assert val2.useconds == usecs


def test_boolean_nominal():
    """ Tests the nominal cases of a BoolType """
    valid_values_test(BoolType, [True, False], 1)


def test_boolean_off_nominal():
    """ Tests the nominal cases of a BoolType """
    invalid_values_test(
        BoolType, filter(lambda item: not isinstance(item, bool), PYTHON_TESTABLE_TYPES)
    )


def test_int_types_nominal():
    """ Tests the integer types """
    for type_input, size in [(I8Type, 1), (I16Type, 2), (I32Type, 4), (I64Type, 8)]:
        total = pow(2, (size * 8) - 1)
        valid_values_test(type_input, [0, -1, 1, -total, total - 1], size)


def test_int_types_off_nominal():
    """ Tests the integer off nominal types """
    for type_input, size in [(I8Type, 1), (I16Type, 2), (I32Type, 4), (I64Type, 8)]:
        total = pow(2, (size * 8) - 1)
        invalid_values_test(
            type_input,
            filter(lambda item: not isinstance(item, int), PYTHON_TESTABLE_TYPES),
        )
        invalid_values_test(
            type_input, [-total - 1, total, -total * 35, total * 35], TypeRangeException
        )


def test_uint_types_nominal():
    """ Tests the integer types """
    for type_input, size in [(U8Type, 1), (U16Type, 2), (U32Type, 4), (U64Type, 8)]:
        max_int = pow(2, (size * 8)) - 1
        valid_values_test(type_input, [0, 1, max_int - 1, max_int], size)


def test_uint_types_off_nominal():
    """ Tests the integer off nominal types """
    for type_input, size in [(U8Type, 1), (U16Type, 2), (U32Type, 4), (U64Type, 8)]:
        max_int = pow(2, (size * 8)) - 1
        invalid_values_test(
            type_input,
            filter(lambda item: not isinstance(item, int), PYTHON_TESTABLE_TYPES),
        )
        invalid_values_test(
            type_input,
            [-1, -2, max_int + 1, max_int * 35, -max_int],
            TypeRangeException,
        )


def test_float_types_nominal():
    """ Tests the integer types """
    valid_values_test(F32Type, [0.31415000557899475, 0.0, -3.141590118408203], 4)
    valid_values_test(F64Type, [0.31415000557899475, 0.0, -3.141590118408203], 8)


def test_float_types_off_nominal():
    """ Tests the integer off nominal types """
    invalid_values_test(
        F32Type, filter(lambda item: not isinstance(item, float), PYTHON_TESTABLE_TYPES)
    )
    invalid_values_test(
        F64Type, filter(lambda item: not isinstance(item, float), PYTHON_TESTABLE_TYPES)
    )


def test_enum_type():
    """
    Tests the EnumType serialization and deserialization
    """
    members = {"MEMB1": 0, "MEMB2": 6, "MEMB3": 9}
    val1 = EnumType("SomeEnum", members, "MEMB3")
    buff = val1.serialize()
    val2 = EnumType("SomeEnum", members)
    val2.deserialize(buff, 0)
    assert val1.val == val2.val


def check_cloned_member_list(members1, members2):
    """ Check member list knowing direct compares don't work"""
    for tuple1, tuple2 in zip(members1, members2):
        assert tuple1[0] == tuple2[0], "Names do not match"
        assert tuple1[2] == tuple2[2], "Format strings do not match"
        assert tuple1[3] == tuple2[3], "Descriptions do not match"
        assert tuple1[1].val == tuple2[1].val, "Values don't match"


def test_serializable_type():
    """
    Tests the SerializableType serialization and deserialization
    """
    u32Mem = U32Type(1000000)
    stringMem = StringType("something to say")
    members = {"MEMB1": 0, "MEMB2": 6, "MEMB3": 9}
    enumMem = EnumType("SomeEnum", members, "MEMB3")
    memList = [
        ("mem1", u32Mem, ">i"),
        ("mem2", stringMem, ">H"),
        ("mem3", enumMem, ">i"),
    ]
    serType1 = SerializableType("ASerType", memList)
    buff = serType1.serialize()
    serType2 = SerializableType("ASerType", memList)
    serType2.deserialize(buff, 0)
    check_cloned_member_list(serType1.mem_list, serType2.mem_list)

    assert serType1.val == serType2.val

    i32Mem = I32Type(-1000000)
    stringMem = StringType("something else to say")
    members = {"MEMB1": 4, "MEMB2": 2, "MEMB3": 0}
    enumMem = EnumType("SomeEnum", members, "MEMB3")
    memList = [
        ("mem1", i32Mem, ">i"),
        ("mem2", stringMem, ">H"),
        ("mem3", enumMem, ">i"),
    ]
    serType1 = SerializableType("ASerType", memList)
    buff = serType1.serialize()
    serType2 = SerializableType("ASerType", memList)
    serType2.deserialize(buff, 0)
    check_cloned_member_list(serType1.mem_list, serType2.mem_list)

    value_dict = {"mem1": 3, "mem2": "abc 123", "mem3": "MEMB1"}
    serType1.val = value_dict
    assert serType1.val == value_dict
    mem_list = serType1.mem_list
    memList = [(a, b, c, None) for a, b, c in memList]
    check_cloned_member_list(mem_list, memList)


# def test_array_type():
#    """
#    Tests the ArrayType serialization and deserialization
#    """
#    extra_ctor_args = [("TestArray", (I32Type, 2, "I DON'T KNOW")), ("TesyArray2", (U8Type, 4, "I DON'T KNOW")),
#               ("TesyArray3", (StringType, 1, "I DON'T KNOW"))]
#    values = [[32, 1], [0, 1, 2, 3], ["one"]]
#    sizes = [8, 4, 3]
#
#    valid_values_test(ArrayType, values, sizes, extra_ctor_args)


def test_time_type():
    """
    Tests the TimeType serialization and deserialization
    """
    TIME_SIZE = 11

    in_no_err_list = [
        (TimeBase["TB_NONE"].value, 1, 100, 999999),
        (TimeBase["TB_PROC_TIME"].value, 0xFF, 1234567, 2952),
        (TimeBase["TB_WORKSTATION_TIME"].value, 8, 1529430215, 12),
        (TimeBase["TB_SC_TIME"].value, 231, 1344230277, 123456),
        (TimeBase["TB_FPGA_TIME"].value, 78, 10395, 24556),
        (TimeBase["TB_DONT_CARE"].value, 0xB3, 12390819, 12356),
    ]

    in_err_list = [
        (10, 58, 15345, 0),
        (TimeBase["TB_NONE"].value, 1, 3, -1),
        (TimeBase["TB_WORKSTATION_TIME"].value, 1, 700000, 1234567),
    ]

    val = TimeType()
    size = val.getSize()
    assert size == TIME_SIZE

    for (t_base, t_context, secs, usecs) in in_no_err_list:
        ser_deser_time_test(t_base, t_context, secs, usecs)

    for (t_base, t_context, secs, usecs) in in_err_list:
        with pytest.raises(TypeRangeException):
            ser_deser_time_test(t_base, t_context, secs, usecs)
