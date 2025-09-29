// ======================================================================
// \title  IpCfg.hpp
// \author mstarch
// \brief  hpp file for SocketIpDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#include <Fw/Time/TimeInterval.hpp>
#ifndef REF_IPCFG_HPP
#define REF_IPCFG_HPP

enum IpCfg {
    SOCKET_SEND_TIMEOUT_SECONDS = 1,       // Seconds component of timeout to an individual send
    SOCKET_SEND_TIMEOUT_MICROSECONDS = 0,  // Milliseconds component of timeout to an individual send
    SOCKET_IP_SEND_FLAGS = 0,              // send, sendto FLAGS argument
    SOCKET_IP_RECV_FLAGS = 0,              // recv FLAGS argument
    SOCKET_MAX_ITERATIONS = 0xFFFF,        // Maximum send/recv attempts before an error is returned
    SOCKET_MAX_HOSTNAME_SIZE = 256         // Maximum stored hostname
};
static const Fw::TimeInterval SOCKET_RETRY_INTERVAL = Fw::TimeInterval(1, 0);

#ifdef TGT_OS_TYPE_VXWORKS
#include <socket.h>
#elif defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
#include <sys/socket.h>
#else
#error OS not supported for IP Socket Communications
#endif

// Value type enumeration
enum SocketOptionValueType {
    SOCK_OPT_INT,     // Integer value
    SOCK_OPT_SIZE_T,  // size_t value
};

// Socket option structure with flexible value types
struct IpSocketOptions {
    int option;                  // Socket option name
    int level;                   // Socket level
    SocketOptionValueType type;  // Type of value stored

    union {
        int intVal;      // Integer value
        size_t sizeVal;  // Size_t value
    } value;
};

// Helper functions to create different types of socket options
inline IpSocketOptions makeIntOption(int opt, int level, int val) {
    IpSocketOptions option;
    option.option = opt;
    option.level = level;
    option.type = SOCK_OPT_INT;
    option.value.intVal = val;
    return option;
}

inline IpSocketOptions makeSizeOption(int opt, int level, size_t val) {
    IpSocketOptions option;
    option.option = opt;
    option.level = level;
    option.type = SOCK_OPT_SIZE_T;
    option.value.sizeVal = val;
    return option;
}

// Array of socket options to explicitly set using setsockopt
// Use the dedicated helper functions to create options of the correct type
//          makeIntOption -> for int values
//          makeSizeOption -> for size_t values
// NOTE: Socket options should be chosen based on project needs and with
// consideration of their implications with regard to security.
// For example, if enabling S_REUSEADDR and there is a hostile actor on the same
// machine, they could potentially bind to the same port and intercept messages.
// Projects should evaluate their threat model and choose options accordingly.
static const IpSocketOptions IP_SOCKET_OPTIONS[] = {
    makeIntOption(SO_REUSEADDR, SOL_SOCKET, 0),  // Example
    // Add other socket options as needed, and expand above helper functions
    // if other types are needed
};

#endif  // REF_IPCFG_HPP
