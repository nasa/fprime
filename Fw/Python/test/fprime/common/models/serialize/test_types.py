"""
Created on Jun 23, 2020
@author: tcanham, reder, hpaulson
"""
from __future__ import print_function
from __future__ import absolute_import

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
from fprime.common.models.serialize.u8_type import U8Type
from fprime.common.models.serialize.u16_type import U16Type
from fprime.common.models.serialize.u32_type import U32Type
from fprime.common.models.serialize.u64_type import U64Type

from fprime.common.models.serialize.type_exceptions import TypeException


if __name__ == "__main__":

    # test_bool_type
    print("\nBOOL")
    try:
        val = BoolType(True)
        print("Value: %s" % str(val.val))
        buff = val.serialize()
        type_base.showBytes(buff)
        val2 = BoolType()
        val2.deserialize(buff, 0)
        print("Deserialized: %s" % str(val2.val))
    except TypeException as e:
        print("Exception: %s" % e.getMsg())

    # test_enum_type
    print("\nENUM")
    try:
        members = {"MEMB1": 0, "MEMB2": 6, "MEMB3": 9}
        print("Members: ", members)
        val = EnumType("SomeEnum", members, "MEMB3")
        print("Value: %s" % val.val)
        buff = val.serialize()
        type_base.showBytes(buff)
        val2 = EnumType("SomeEnum", members)
        val2.deserialize(buff, 0)
        print("Deserialize: %s" % val2.val)
    except TypeException as e:
        print("Exception: %s" % e.getMsg())

    # test_f32_type
    print("\nF32")
    try:
        val = F32Type(3.6)
        print("Value: %s" % str(val.val))
        buff = val.serialize()
        type_base.showBytes(buff)
        print("Serialized: ", repr(buff))
        val2 = F32Type()
        val2.deserialize(buff, 0)
        print("Deserialize: %f" % val2.val)
    except TypeException as e:
        print("Exception: %s" % e.getMsg())

    # test_f64_type
    print("\nF64")
    try:
        val = F64Type(100000.23)
        print("Value: %s" % str(val.val))
        buff = val.serialize()
        type_base.showBytes(buff)
        print("Serialized: ", repr(buff))
        val2 = F64Type()
        val2.deserialize(buff, 0)
        print("Deserialize: %f" % val2.val)
    except TypeException as e:
        print("Exception: %s" % e.getMsg())

    # test_i8_type
    print("\nI8")
    try:
        val = I8Type(-2)
        print("Value: %s" % str(val.val))
        buff = val.serialize()
        type_base.showBytes(buff)
        print("Serialized: ", repr(buff))
        val2 = I8Type()
        val2.deserialize(buff, 0)
        print("Deserialize: %s" % str(val2.val))
    except TypeException as e:
        print("Exception: %s" % e.getMsg())

    # test_i16_type
    print("\nI16")
    try:
        val = I16Type(-2000)
        print("Value: %s" % str(val.val))
        buff = val.serialize()
        type_base.showBytes(buff)
        print("Serialized: ", repr(buff))
        val2 = I16Type()
        val2.deserialize(buff, 0)
        print("Deserialize: %s" % str(val2.val))
    except TypeException as e:
        print("Exception: %s" % e.getMsg())

    # test_i32_type
    print("\nI32")
    try:
        val = I32Type(-2000000)
        print("Value: %s" % str(val.val))
        buff = val.serialize()
        type_base.showBytes(buff)
        print("Serialized: ", repr(buff))
        val2 = I32Type()
        val2.deserialize(buff, 0)
        print("Deserialize: %s" % str(val2.val))
        #
        val3 = I32Type(1000000)
        print("Value: %s" % str(val3.val))
        buff = val3.serialize()
        type_base.showBytes(buff)
        print("Serialized: ", repr(buff))
        val4 = I32Type()
        val4.deserialize(buff, 0)
        print("Deserialize: %s" % str(val4.val))

    except TypeException as e:
        print("Exception: %s" % e.getMsg())

    # test_i64_type
    print("\nI64")
    try:
        val = I64Type(-2000000)
        print("Value: %s" % str(val.val))
        buff = val.serialize()
        type_base.showBytes(buff)
        print("Serialized: ", repr(buff))
        val2 = I64Type()
        val2.deserialize(buff, 0)
        print("Deserialize: %s" % str(val2.val))
    except TypeException as e:
        print("Exception: %s" % e.getMsg())

    # test_serializable_type
    print("\nSerializable")
    try:
        i32Mem = U32Type(1000000)
        stringMem = StringType("something to say")
        members = {"MEMB1": 0, "MEMB2": 6, "MEMB3": 9}
        enumMem = EnumType("SomeEnum", members, "MEMB3")
        memList = [
            ("mem1", i32Mem, ">i"),
            ("mem2", stringMem, ">H"),
            ("mem3", enumMem, ">i"),
        ]
        print("Value: %s" % repr(memList))
        serType = SerializableType("ASerType", memList)
        buff = serType.serialize()
        type_base.showBytes(buff)
        print("Serialized: ", repr(buff))
        serType2 = SerializableType("ASerType", memList)
        serType2.deserialize(buff, 0)
        print("Deserialized: %s" % repr(serType2.mem_list))
    except TypeException as e:
        print("Exception: %s" % e.getMsg())

    # test_string_type
    print("\nString")
    try:
        val = StringType("This is a string")
        print("Value: %s" % val.val)
        buff = val.serialize()
        type_base.showBytes(buff)
        val2 = StringType()
        val2.deserialize(buff, 0)
        print("Deserialize: %s" % val2.val)
    except TypeException as e:
        print("Exception: %s" % e.getMsg())

    # test_time_type
    # already has its own unit test file

    # test_u8_type
    print("\nU8")
    try:
        val = U8Type(8)
        print("Value: %s" % str(val.val))
        buff = val.serialize()
        type_base.showBytes(buff)
        print("Serialized: ", repr(buff))
        val2 = U8Type()
        val2.deserialize(buff, 0)
        print("Deserialized: %s" % str(val2.val))
    except TypeException as e:
        print("Exception: %s" % e.getMsg())

    # test_u16_type
    print("\nU16")
    try:
        val = U16Type(1000)
        print("Val: %s" % str(val.val))
        buff = val.serialize()
        type_base.showBytes(buff)
        print("Serialized: ", repr(buff))
        val2 = U16Type()
        val2.deserialize(buff, 0)
        print("Deser: %s" % str(val2.val))
    except TypeException as e:
        print("Exception: %s" % e.getMsg())

    # test_u32_type
    print("\nU32")
    try:
        val = U32Type(1000000)
        print("Value: %s" % str(val.val))
        buff = val.serialize()
        type_base.showBytes(buff)
        print("Serialized: ", repr(buff))
        val2 = U32Type()
        val2.deserialize(buff, 0)
        print("Deserialize: %s" % str(val2.val))
    except TypeException as e:
        print("Exception: %s" % e.getMsg())

    # test_u64_type
    print("\nU64")
    try:
        val = U64Type(1000000)
        print("Value: %s" % str(val.val))
        buff = val.serialize()
        type_base.showBytes(buff)
        print("Serialized: ", repr(buff))
        val2 = U64Type()
        val2.deserialize(buff, 0)
        print("Deserialized: %s" % str(val2.val))
    except TypeException as e:
        print("Exception: %s" % e.getMsg())
