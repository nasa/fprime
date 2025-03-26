@ The type of a telemetry channel identifier
type FwChanIdType = U32

@ The type of a data product identifier
type FwDpIdType = U32

@ The type of a data product priority
type FwDpPriorityType = U32

@ The type of an event identifier
type FwEventIdType = U32

@ The type of a command opcode
type FwOpcodeType = U32

@ The type of a com packet descriptor
type FwPacketDescriptorType = U32

@ The type of a parameter identifier
type FwPrmIdType = U32

@ The type used to serialize a size value
type FwSizeStoreType = U16

@ The unsigned type of larger sizes internal to the software,
@ e.g., memory buffer sizes, file sizes
type FwSizeType = U32

@ The type used to serialize a time base value
type FwTimeBaseStoreType = U16

@ The type used to serialize a time context value
type FwTimeContextStoreType = U8

@ The type of a telemetry packet identifier
type FwTlmPacketizeIdType = U16

@ The type of a trace identifier
type FwTraceIdType = U32

@ The type used to serialize a C++ enumeration constant
@ FPP enumerations are serialized according to their representation types
type FwEnumStoreType = I32
