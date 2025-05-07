# ======================================================================
# \title  config/FpConfig.fpp
# \author mstarch
# \brief  FPP alias configuration file
#
# \copyright
# Copyright 2025, by the California Institute of Technology.
# ALL RIGHTS RESERVED.  United States Government Sponsorship
# acknowledged.
#
# FPrime uses FPP to define a set of type aliases for various named types
# used throughout the system. This file is used to configure those types.
# ======================================================================

####
# Integer type aliases:
# Used for the project to override types supplied by the platform for things like sizes, indices, etc.
####

@ The unsigned type of larger sizes internal to the software,
@ e.g., memory buffer sizes, file sizes. Must be unsigned.
type FwSizeType = PlatformSizeType

@ The signed type of larger sizes internal to the software, used
@ for signed offsets, e.g., file seek offsets. Must be signed.
type FwSignedSizeType = PlatformSignedSizeType

@ The type of smaller indices internal to the software, used
@ for array indices, e.g., port indices. Must be signed.
type FwIndexType = PlatformIndexType

@ The type of arguments to assert functions.
type FwAssertArgType = PlatformAssertArgType

@ The type of task priorities used.
type FwTaskPriorityType = PlatformTaskPriorityType

@ The type of queue priorities used.
type FwQueuePriorityType = PlatformQueuePriorityType


####
# GDS type aliases:
# Used for the project to override types shared with GDSes and other remote systems.
####

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
