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

@ The id type.
type FwIdType = U32

@ The type of task priorities used.
type FwTaskIdType = PlatformTaskIdType

####
# GDS type aliases:
# Used for the project to override types shared with GDSes and other remote systems.
####

@ The type of a telemetry channel identifier
type FwChanIdType = FwIdType

@ The type of a data product identifier
type FwDpIdType = FwIdType

@ The type of a data product priority
type FwDpPriorityType = U32

@ The type of an event identifier
type FwEventIdType = FwIdType

@ The type of a command opcode
type FwOpcodeType = FwIdType

@ The type of a parameter identifier
type FwPrmIdType = FwIdType

@ The type used to serialize a size value
type FwSizeStoreType = U16


@ The type used to serialize a time context value
type FwTimeContextStoreType = U8

@ The type of a telemetry packet identifier
type FwTlmPacketizeIdType = U16

@ The type of a trace identifier
type FwTraceIdType = U32

@ The type used to serialize a C++ enumeration constant
@ FPP enumerations are serialized according to their representation types
type FwEnumStoreType = I32


@ The type used to serialize a time base value
type FwTimeBaseStoreType = U16

@ Define enumeration for Time base types
enum TimeBase : FwTimeBaseStoreType {
    TB_NONE = 0              @< No time base has been established (Required)
    TB_PROC_TIME = 1         @< Indicates time is processor cycle time. Not tied to external time
    TB_WORKSTATION_TIME = 2  @< Time as reported on workstation where software is running. For testing. (Required)
    TB_DONT_CARE = 0xFFFF    @< Don't care value for sequences. If FwTimeBaseStoreType is changed, value should be changed (Required)
} default TB_NONE;
