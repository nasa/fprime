/**
 * \file
 * \author T. Canham
 * \brief ISF configuration file
 *
 * \copyright
 * Copyright 2009-2015, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 * <br /><br />
 */
#ifndef _FW_CONFIG_HPP_
#define _FW_CONFIG_HPP_

// To enable various facilities, set the below to 0 or 1. If it is set in compiler flags,
// these defaults will be overridden

// Available types

#ifndef FW_HAS_64_BIT
#define FW_HAS_64_BIT                       1  //!< Architecture supports 64 bit integers
#endif

#ifndef FW_HAS_32_BIT
#define FW_HAS_32_BIT                       1  //!< Architecture supports 32 bit integers
#endif

#ifndef FW_HAS_16_BIT
#define FW_HAS_16_BIT                       1  //!< Architecture supports 16 bit integers
#endif

#ifndef FW_HAS_F64
#define FW_HAS_F64                          1  //!< Architecture supports 64 bit floating point numbers
#endif

// Boolean values for serialization

#ifndef FW_SERIALIZE_TRUE_VALUE
#define FW_SERIALIZE_TRUE_VALUE             (0xFF)  //!< Value encoded during serialization for boolean true
#endif

#ifndef FW_SERIALIZE_FALSE_VALUE
#define FW_SERIALIZE_FALSE_VALUE             (0x00) //!< Value encoded during serialization for boolean false
#endif

#ifndef AssertArg
#define AssertArg U32
#endif

// typedefs for various serialization items
// *** NOTE *** Changes here MUST match GSE in order to decode the values correctly

#ifndef FwPacketDescriptorType
#define FwPacketDescriptorType U32          //!< Type representation for a packet descriptor
#endif

#ifndef FwOpcodeType
#define FwOpcodeType U32                    //!< Type representation for a command opcode
#endif

#ifndef FwChanIdType
#define FwChanIdType U32                    //!< Type representation for a channel id
#endif

#ifndef FwEventIdType
#define FwEventIdType U32                   //!< Type representation for an event id
#endif

#ifndef FwPrmIdType
#define FwPrmIdType U32                     //!< Type representation for a parameter id
#endif

#ifndef FwTlmPacketizeIdType
#define FwTlmPacketizeIdType U16            //!< Packetized telemetry packet id
#endif

// How big the size of a buffer (or string) representation is
#ifndef FwBuffSizeType
#define FwBuffSizeType U16                  //!< Type representation for storing a buffer or string size
#endif

// How many bits are used to store an enumeration defined in XML during serialization.
#ifndef FwEnumStoreType
#define FwEnumStoreType I32                 //!< Type representation for an enumeration value
#endif

// Object facilities

// Allow objects to have names. Allocates storage for each instance
#ifndef FW_OBJECT_NAMES
#define FW_OBJECT_NAMES                     1   //!< Indicates whether or not object names are stored (more memory, can be used for tracking objects)
#endif

// To reduce binary size, FW_OPTIONAL_NAME(<string>) can be used to substitute strings with an empty string
// when running with FW_OBJECT_NAMES disabled
#if FW_OBJECT_NAMES == 1
 #define FW_OPTIONAL_NAME(name) name
#else
 #define FW_OPTIONAL_NAME(name) ""
#endif

// Add methods to query an object about its name. Can be overridden by derived classes
// For FW_OBJECT_TO_STRING to work, FW_OBJECT_NAMES must be enabled
#if FW_OBJECT_NAMES == 1
 #ifndef FW_OBJECT_TO_STRING
 #define FW_OBJECT_TO_STRING                1   //!< Indicates whether or not generated objects have toString() methods to dump internals (more code)
 #endif
#else
 #define FW_OBJECT_TO_STRING                0
#endif

// Adds the ability for all component related objects to register
// centrally.
#ifndef FW_OBJECT_REGISTRATION
#define FW_OBJECT_REGISTRATION              1   //!< Indicates whether or not objects can register themselves (more code, more object tracking)
#endif

#ifndef FW_QUEUE_REGISTRATION
#define FW_QUEUE_REGISTRATION               1   //!< Indicates whether or not queue registration is used
#endif

#ifndef FW_BAREMETAL_SCHEDULER
#define FW_BAREMETAL_SCHEDULER             0   //!< Indicates whether or not a baremetal scheduler should be used. Alternatively the Os scheduler is used.
#endif

// Port Facilities

// This allows tracing calls through ports for debugging
#ifndef FW_PORT_TRACING
#define FW_PORT_TRACING                     1   //!< Indicates whether port calls are traced (more code, more visibility into execution)
#endif

// This generates code to connect to serialized ports
#ifndef FW_PORT_SERIALIZATION
#define FW_PORT_SERIALIZATION               1   //!< Indicates whether there is code in ports to serialize the call (more code, but ability to serialize calls for multi-note systems)
#endif

// Component Facilities

// Serialization

// Add a type id when serialization is done. More storage,
// but better detection of errors
// TODO: Not working yet

#ifndef FW_SERIALIZATION_TYPE_ID
#define FW_SERIALIZATION_TYPE_ID            0   //!< Indicates if type id is stored when type is serialized. (More storage, but more type safety)
#endif

// Number of bytes to use for serialization IDs. More
// bytes is more storage, but greater number of IDs
#if FW_SERIALIZATION_TYPE_ID
 #ifndef FW_SERIALIZATION_TYPE_ID_BYTES
 #define FW_SERIALIZATION_TYPE_ID_BYTES      4  //!< Number of bytes used to represent type id - more bytes, more ids
 #endif
#endif

// Turn asserts on or off

#define FW_NO_ASSERT                        1   //!< Asserts turned off
#define FW_FILEID_ASSERT                    2   //!< File ID used - requires -DASSERT_FILE_ID=somevalue to be set on the compile command line
#define FW_FILENAME_ASSERT                  3   //!< Uses the file name in the assert - image stores filenames
#define FW_ASSERT_DFL_MSG_LEN               256 //!< Maximum assert message length when using the default assert handler

#ifndef FW_ASSERT_LEVEL
#define FW_ASSERT_LEVEL                     FW_FILENAME_ASSERT //!< Defines the type of assert used
#endif

// Define max length of assert string
#ifndef FW_ASSERT_TEXT_SIZE
#define FW_ASSERT_TEXT_SIZE                 120  //!< Size of string used to store assert description
#endif

// Adjust various configuration parameters in the architecture. Some of the above enables may disable some of the values

// The size of the object name stored in the object base class. Larger names will be truncated.
#if FW_OBJECT_NAMES
 #ifndef FW_OBJ_NAME_MAX_SIZE
 #define FW_OBJ_NAME_MAX_SIZE                80  //!< Size of object name (if object names enabled). AC Limits to 80, truncation occurs above 80.
 #endif
#endif

// When querying an object as to an object-specific description, this specifies the size of the buffer to store the description.
#if FW_OBJECT_TO_STRING
 #ifndef FW_OBJ_TO_STRING_BUFFER_SIZE
 #define FW_OBJ_TO_STRING_BUFFER_SIZE        255  //!< Size of string storing toString() text
 #endif
#endif

#if FW_OBJECT_REGISTRATION
// For the simple object registry provided with the framework, this specifies how many objects the registry will store.
 #ifndef FW_OBJ_SIMPLE_REG_ENTRIES
 #define FW_OBJ_SIMPLE_REG_ENTRIES           500  //!< Number of objects stored in simple object registry
 #endif
// When dumping the contents of the registry, this specifies the size of the buffer used to store object names. Should be >= FW_OBJ_NAME_MAX_SIZE.
 #ifndef FW_OBJ_SIMPLE_REG_BUFF_SIZE
 #define FW_OBJ_SIMPLE_REG_BUFF_SIZE         255  //!< Size of object registry dump string
 #endif
#endif

#if FW_QUEUE_REGISTRATION
// For the simple queue registry provided with the framework, this specifies how many queues the registry will store.
 #ifndef FW_QUEUE_SIMPLE_QUEUE_ENTRIES
 #define FW_QUEUE_SIMPLE_QUEUE_ENTRIES           100  //!< Number of queues stored in simple queue registry
 #endif
#endif


// Specifies the size of the string holding the queue name for queues
#ifndef FW_QUEUE_NAME_MAX_SIZE
#define FW_QUEUE_NAME_MAX_SIZE               80   //!< Max size of message queue name
#endif

// Specifies the size of the string holding the task name for active components and tasks
#ifndef FW_TASK_NAME_MAX_SIZE
#define FW_TASK_NAME_MAX_SIZE               80    //!< Max size of task name
#endif

// Specifies the size of the buffer that contains a communications packet.
#ifndef FW_COM_BUFFER_MAX_SIZE
#define FW_COM_BUFFER_MAX_SIZE               128   //!< Max size of Fw::Com buffer
#endif

// Specifies the size of the buffer that contains the serialized command arguments.

#ifndef FW_CMD_ARG_BUFFER_MAX_SIZE
#define FW_CMD_ARG_BUFFER_MAX_SIZE           (FW_COM_BUFFER_MAX_SIZE - sizeof(FwOpcodeType) - sizeof(FwPacketDescriptorType))
#endif

// Specifies the maximum size of a string in a command argument
#ifndef FW_CMD_STRING_MAX_SIZE
#define FW_CMD_STRING_MAX_SIZE           40   //!< Max character size of command string arguments
#endif

// Normally when a command is deserialized, the handler checks to see if there are any leftover
// bytes in the buffer. If there are, it assumes that the command was corrupted somehow since
// the serialized size should match the serialized size of the argument list. In some cases,
// command buffers are padded so the data can be larger than the serialized size of the command.
// Setting the below to zero will disable the check at the cost of not detecting commands that
// are too large.
#ifndef FW_CMD_CHECK_RESIDUAL
#define FW_CMD_CHECK_RESIDUAL               1 //!< Check for leftover command bytes
#endif

// Specifies the size of the buffer that contains the serialized log arguments.
#ifndef FW_LOG_BUFFER_MAX_SIZE
#define FW_LOG_BUFFER_MAX_SIZE               (FW_COM_BUFFER_MAX_SIZE - sizeof(FwEventIdType) - sizeof(FwPacketDescriptorType))
#endif

// Specifies the maximum size of a string in a log event
#ifndef FW_LOG_STRING_MAX_SIZE
#define FW_LOG_STRING_MAX_SIZE           100   //!< Max size of log string parameter type
#endif

// Specifies the size of the buffer that contains the serialized telemetry value.
#ifndef FW_TLM_BUFFER_MAX_SIZE
#define FW_TLM_BUFFER_MAX_SIZE               (FW_COM_BUFFER_MAX_SIZE - sizeof(FwChanIdType) - sizeof(FwPacketDescriptorType))
#endif

// Specifies the maximum size of a string in a telemetry channel
#ifndef FW_TLM_STRING_MAX_SIZE
#define FW_TLM_STRING_MAX_SIZE           40  //!< Max size of channelized telemetry string type
#endif

// Specifies the size of the buffer that contains the serialized parameter value.
#ifndef FW_PARAM_BUFFER_MAX_SIZE
#define FW_PARAM_BUFFER_MAX_SIZE             (FW_COM_BUFFER_MAX_SIZE - sizeof(FwPrmIdType) - sizeof(FwPacketDescriptorType))
#endif

// Specifies the maximum size of a string in a parameter
#ifndef FW_PARAM_STRING_MAX_SIZE
#define FW_PARAM_STRING_MAX_SIZE           40  //!< Max size of parameter string type
#endif

// Specifies the maximum size of a file upload chunk
#ifndef FW_FILE_BUFFER_MAX_SIZE
#define FW_FILE_BUFFER_MAX_SIZE             255   //!< Max size of file buffer (i.e. chunk of file)
#endif

// Specifies the maximum size of a string in an interface call
#ifndef FW_INTERNAL_INTERFACE_STRING_MAX_SIZE
#define FW_INTERNAL_INTERFACE_STRING_MAX_SIZE           256   //!< Max size of interface string parameter type
#endif

// enables text logging of events as well as data logging. Adds a second logging port for text output.
#ifndef FW_ENABLE_TEXT_LOGGING
#define FW_ENABLE_TEXT_LOGGING				 1    //!< Indicates whether text logging is turned on
#endif

 // Define the size of the text log string buffer. Should be large enough for format string and arguments
#ifndef FW_LOG_TEXT_BUFFER_SIZE
#define FW_LOG_TEXT_BUFFER_SIZE              256   //!< Max size of string for text log message
#endif

// Define if serializables have toString() method. Turning off will save code space and
// string constants. Must be enabled if text logging enabled
#ifndef FW_SERIALIZABLE_TO_STRING
#define FW_SERIALIZABLE_TO_STRING			1    //!< Indicates if autocoded serializables have toString() methods
#endif

#if FW_SERIALIZABLE_TO_STRING
#ifndef FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE
#define FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE 255   //!< Size of string to store toString() string output
#endif
#endif

// Define if arrays have toString() method.
#ifndef FW_ARRAY_TO_STRING
#define FW_ARRAY_TO_STRING                  1   //!< Indicates if autocoded arrays have toString() methods
#endif

#if FW_ARRAY_TO_STRING
#ifndef FW_ARRAY_TO_STRING_BUFFER_SIZE
#define FW_ARRAY_TO_STRING_BUFFER_SIZE 256  //!< Size of string to store toString() string output
#endif
#endif

// Some settings to enable AMPCS compatibility. This breaks regular ISF GUI compatibility
#ifndef FW_AMPCS_COMPATIBLE
#define FW_AMPCS_COMPATIBLE            0   //!< Whether or not JPL AMPCS ground system support is enabled.
#endif

// Define enumeration for Time base types
enum TimeBase {
    TB_NONE, //!< No time base has been established
    TB_PROC_TIME, //!< Indicates time is processor cycle time. Not tied to external time
    TB_WORKSTATION_TIME, //!< Time as reported on workstation where software is running. For testing.
    TB_DONT_CARE = 0xFFFF //!< Don't care value for sequences. If FwTimeBaseStoreType is changed, value should be changed
};

// How many bits are used to store the time base
#ifndef FwTimeBaseStoreType
#define FwTimeBaseStoreType U16                 //!< Storage conversion for time base in scripts/ground interface
#endif

#ifndef FwTimeContextStoreType
#define FwTimeContextStoreType U8                 //!< Storage conversion for time context in scripts/ground interface
#define FW_CONTEXT_DONT_CARE 0xFF                 //!< Don't care value for time contexts in sequences
#endif

// These settings configure whether or not the timebase and context values for the Fw::Time
// class are used. Some systems may not use or need those fields

#ifndef FW_USE_TIME_BASE
#define FW_USE_TIME_BASE                1 //!< Whether or not to use the time base
#endif

#ifndef FW_USE_TIME_CONTEXT
#define FW_USE_TIME_CONTEXT             1 //!< Whether or not to serialize the time context
#endif
//
//These defines used for the FilepathCharString type

#ifndef FW_FIXED_LENGTH_STRING_SIZE
#define FW_FIXED_LENGTH_STRING_SIZE   256 //!< Character array size for the filepath character type
#endif

// *** NOTE configuration checks are in Fw/Cfg/ConfigCheck.cpp in order to have
// the type definitions in Fw/Types/BasicTypes available.

#endif
