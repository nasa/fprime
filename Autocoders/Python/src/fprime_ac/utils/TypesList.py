# types allowed in commands/telemetry/parameters/events
types_list = [
    "bool",
    "char",
    "F32",
    "F64",
    "I16",
    "I32",
    "I64",
    "I8",
    "U16",
    "U32",
    "U64",
    "U8",
]

# additional types allowed in ports
port_types_list = [
    "FwBuffSizeType",
    "FwChanIdType",
    "FwEnumStoreType",
    "FwEventIdType",
    "FwIndexType",
    "FwOpcodeType",
    "FwPacketDescriptorType",
    "FwPrmIdType",
    "FwSizeType",
    "FwTimeBaseStoreType",
    "FwTimeContextStoreType",
    "FwTlmPacketizeIdType",
    "NATIVE_INT_TYPE",
    "NATIVE_UINT_TYPE",
    "POINTER_CAST",
]


def isPrimitiveType(type):
    return type in (types_list + port_types_list)
