/**
 * \file: FpConfig.h
 * \author T. Canham, mstarch
 * \brief C-compatible configuration header for fprime configuration
 *
 * \copyright
 * Copyright 2009-2015, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 */
#ifndef FPCONFIG_H_
#define FPCONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <Fw/Types/BasicTypes.h>
#include <Platform/PlatformTypes.h>

// ----------------------------------------------------------------------
// Configuration switches
// ----------------------------------------------------------------------

// Allow objects to have names. Allocates storage for each instance
#ifndef FW_OBJECT_NAMES
#define FW_OBJECT_NAMES \
    1  //!< Indicates whether or not object names are stored (more memory, can be used for tracking objects)
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
#define FW_OBJECT_TO_STRING \
    1  //!< Indicates whether or not generated objects have toString() methods to dump internals (more code)
#endif
#else
#define FW_OBJECT_TO_STRING 0
#endif

// Adds the ability for all component related objects to register
// centrally.
#ifndef FW_OBJECT_REGISTRATION
#define FW_OBJECT_REGISTRATION \
    1  //!< Indicates whether or not objects can register themselves (more code, more object tracking)
#endif

#ifndef FW_QUEUE_REGISTRATION
#define FW_QUEUE_REGISTRATION 1  //!< Indicates whether or not queue registration is used
#endif

// On some systems, use of *printf family functions (snprintf, printf, etc) require a prohibitive amount of program
// space. Setting this to `0` indicates that the Fw/String methods should stop using these functions to conserve
// program size. However, this comes at the expense of discarding format parameters. i.e. the format string is returned
// unchanged.
#ifndef FW_USE_PRINTF_FAMILY_FUNCTIONS_IN_STRING_FORMATTING
#define FW_USE_PRINTF_FAMILY_FUNCTIONS_IN_STRING_FORMATTING 1
#endif

// Port Facilities

// This allows tracing calls through ports for debugging
#ifndef FW_PORT_TRACING
#define FW_PORT_TRACING 1  //!< Indicates whether port calls are traced (more code, more visibility into execution)
#endif

// This generates code to connect to serialized ports
#ifndef FW_PORT_SERIALIZATION
#define FW_PORT_SERIALIZATION \
    1  //!< Indicates whether there is code in ports to serialize the call (more code, but ability to serialize
       //!< calls for multi-note systems)
#endif

// Component Facilities

// Serialization

// Add a type id when serialization is done. More storage,
// but better detection of errors
// TODO: Not working yet

#ifndef FW_SERIALIZATION_TYPE_ID
#define FW_SERIALIZATION_TYPE_ID \
    0  //!< Indicates if type id is stored when type is serialized. (More storage, but more type safety)
#endif

// Number of bytes to use for serialization IDs. More
// bytes is more storage, but greater number of IDs
#if FW_SERIALIZATION_TYPE_ID
#ifndef FW_SERIALIZATION_TYPE_ID_BYTES
#define FW_SERIALIZATION_TYPE_ID_BYTES 4  //!< Number of bytes used to represent type id - more bytes, more ids
#endif
#endif

// Set assertion form. Options:
//   1. FW_NO_ASSERT: assertions are compiled out, side effects are kept
//   2. FW_FILEID_ASSERT: asserts report a file CRC and line number
//   3. FW_FILENAME_ASSERT: asserts report a file path (__FILE__) and line number
//   4. FW_RELATIVE_PATH_ASSERT: asserts report a relative path within F' or F' library and line number
//
// Note: users who want alternate asserts should set assert level to FW_NO_ASSERT and define FW_ASSERT in this header
#ifndef FW_ASSERT_LEVEL
#define FW_ASSERT_LEVEL FW_FILENAME_ASSERT  //!< Defines the type of assert used
#endif

// Adjust various configuration parameters in the architecture. Some of the above enables may disable some of the values

// The size of the object name stored in the object base class. Larger names will be truncated.
#if FW_OBJECT_NAMES
#ifndef FW_OBJ_NAME_BUFFER_SIZE
#define FW_OBJ_NAME_BUFFER_SIZE \
    80  //!< Size of object name (if object names enabled). AC Limits to 80, truncation occurs above 80.
#endif
#endif

// Normally when a command is deserialized, the handler checks to see if there are any leftover
// bytes in the buffer. If there are, it assumes that the command was corrupted somehow since
// the serialized size should match the serialized size of the argument list. In some cases,
// command buffers are padded so the data can be larger than the serialized size of the command.
// Setting the below to zero will disable the check at the cost of not detecting commands that
// are too large.
#ifndef FW_CMD_CHECK_RESIDUAL
#define FW_CMD_CHECK_RESIDUAL 1  //!< Check for leftover command bytes
#endif

// Enables text logging of events as well as data logging. Adds a second logging port for text output.
// In order to set this to 0, FPRIME_ENABLE_TEXT_LOGGERS must be set to OFF.
#ifndef FW_ENABLE_TEXT_LOGGING
#define FW_ENABLE_TEXT_LOGGING 1  //!< Indicates whether text logging is turned on
#endif

// Define if serializables have toString() method. Turning off will save code space and
// string constants. Must be enabled if text logging enabled
#ifndef FW_SERIALIZABLE_TO_STRING
#define FW_SERIALIZABLE_TO_STRING 1  //!< Indicates if autocoded serializables have toString() methods
#endif

// Some settings to enable AMPCS compatibility. This breaks regular ISF GUI compatibility
#ifndef FW_AMPCS_COMPATIBLE
#define FW_AMPCS_COMPATIBLE 0  //!< Whether or not JPL AMPCS ground system support is enabled.
#endif

// Posix thread names are limited to 16 characters, this can lead to collisions. In the event of a
// collision, set this to 0.
#ifndef POSIX_THREADS_ENABLE_NAMES
#define POSIX_THREADS_ENABLE_NAMES 1  //!< Enable/Disable assigning names to threads
#endif

// *** NOTE configuration checks are in Fw/Cfg/ConfigCheck.cpp in order to have
// the type definitions in Fw/Types/BasicTypes available.
#ifdef __cplusplus
}
#endif

#endif
