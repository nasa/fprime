"""
(test) bool_type.py

Tests the serialization and deserialization of all the types in Fw/Python/src/fprime/common/models/serialize/

Created on Jun 25, 2020
@author: hpaulson
"""

from __future__ import absolute_import

import pytest

from fprime.common.models.serialize import type_base
from fprime.common.models.serialize.bool_type import BoolType
from fprime.common.models.serialize.enum_type import EnumType
from fprime.common.models.serialize.f32_type import F32Type
from fprime.common.models.serialize.f64_type import F64Type
from fprime.common.models.serialize.i8_type import I8Type
from fprime.common.models.serialize.i16_type import I16Type
from fprime.common.models.serialize.i32_type import I32Type
from fprime.common.models.serialize.i64_type import I64Type
from fprime.common.models.serialize.serializable_type import SerializableType
from fprime.common.models.serialize.string_type import StringType
from fprime.common.models.serialize.time_type import TimeType
from fprime.common.models.serialize.time_type import TimeBase
from fprime.common.models.serialize.time_type import ser_deser_test
from fprime.common.models.serialize.u8_type import U8Type
from fprime.common.models.serialize.u16_type import U16Type
from fprime.common.models.serialize.u32_type import U32Type
from fprime.common.models.serialize.u64_type import U64Type


def test_bool_type():
    """
    Tests the BoolType serialization and deserialization
    """
    val1 = BoolType(True)
    buff = val1.serialize()
    val2 = BoolType()
    val2.deserialize(buff, 0)
    assert val1.val == val2.val

    val1 = BoolType(False)
    buff = val1.serialize()
    val2 = BoolType()
    val2.deserialize(buff, 0)
    assert val1.val == val2.val


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


def test_f32_type():
    """
    Tests the F32Type serialization and deserialization
    """
    val1 = F32Type(6.9)
    buff = val1.serialize()
    val2 = F32Type()
    val2.deserialize(buff, 0)
    assert val1.val == pytest.approx(val2.val, 0.0000001)


def test_f64_type():
    """
    Tests the F64Type serialization and deserialization
    """
    val1 = F64Type(100000.23)
    buff = val1.serialize()
    val2 = F64Type()
    val2.deserialize(buff, 0)
    assert val1.val == pytest.approx(val2.val, 0.0000001)


def test_i8_type():
    """
    Tests the I8Type serialization and deserialization
    """
    val1 = I8Type(-2)
    buff = val1.serialize()
    val2 = I8Type()
    val2.deserialize(buff, 0)
    assert val1.val == val2.val


def test_i16_type():
    """
    Tests the I16Type serialization and deserialization
    """
    val1 = I16Type(-2000)
    buff = val1.serialize()
    val2 = I16Type()
    val2.deserialize(buff, 0)
    assert val1.val == val2.val


def test_i32_type():
    """
    Tests the I32Type serialization and deserialization
    """
    val1 = I32Type(-2000000)
    buff = val1.serialize()
    val2 = I32Type()
    val2.deserialize(buff, 0)
    assert val1.val == val2.val

    val1 = I32Type(1000000)
    buff = val1.serialize()
    val2 = I32Type()
    val2.deserialize(buff, 0)
    assert val1.val == val2.val


def test_i64_type():
    """
    Tests the I64Type serialization and deserialization
    """
    val1 = I64Type(-10000000)
    buff = val1.serialize()
    val2 = I64Type()
    val2.deserialize(buff, 0)
    assert val1.val == val2.val

    val1 = I64Type(10000000)
    buff = val1.serialize()
    val2 = I64Type()
    val2.deserialize(buff, 0)
    assert val1.val == val2.val


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
    assert serType1.mem_list == serType2.mem_list

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
    assert serType1.mem_list == serType2.mem_list


def test_time_type():
    """
    Tests the TimeType serialization and deserialization
    """
    TIME_SIZE = 11
    # test_buff = bytearray(
    #     "\x01\x02\x03\x00\x02\x03\x00\x00\x00\x04\x00\x00\x00\x05", "utf-8"
    # )
    # test_offset = 3
    # test_base = 2
    # test_context = 3
    # test_secs = 4
    # test_usecs = 5

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
        result = ser_deser_test(t_base, t_context, secs, usecs)
        assert result

    for (t_base, t_context, secs, usecs) in in_err_list:
        result = ser_deser_test(t_base, t_context, secs, usecs, should_err=True)
        assert result


def test_u8_type():
    """
    Tests the U8Type serialization and deserialization
    """
    val1 = U8Type(8)
    buff = val1.serialize()
    val2 = U8Type()
    val2.deserialize(buff, 0)
    assert val1.val == val2.val


def test_u16_type():
    """
    Tests the U16Type serialization and deserialization
    """
    val1 = U16Type(1000)
    buff = val1.serialize()
    val2 = U16Type()
    val2.deserialize(buff, 0)
    assert val1.val == val2.val


def test_u32_type():
    """
    Tests the U32Type serialization and deserialization
    """
    val1 = U32Type(1000000)
    buff = val1.serialize()
    val2 = U32Type()
    val2.deserialize(buff, 0)
    assert val1.val == val2.val


def test_u64_type():
    """
    Tests the U64Type serialization and deserialization
    """
    val1 = U64Type(20000000)
    buff = val1.serialize()
    val2 = U64Type()
    val2.deserialize(buff, 0)
    assert val1.val == val2.val
