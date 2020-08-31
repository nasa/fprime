"""Python implementations of built-in fprime types

This package provides the modules necessary for the representation of fprime types (Fw/Types) package in a Python
context. These standard types are represented as Python classes representing each individual type.

Example:
    U32 is represented by fprime.common.modules.serialize.numerical_types.U32Type

Special autocoded types are represented by configurable classes: ArrayType, SerializableType, and EnumType. These
represent a generic version of these classes whose designed properties are set as dynamic properties on the python
class itself.

Example:
    Serializables are represented by fprime.common.modules.serialize.serializable_type.SerializableType
    SerializableType(typename="MySerializable") is used to specify the Ai serializable's name
"""