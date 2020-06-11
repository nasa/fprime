// ======================================================================
// \title  SocketIpDriverCfg.hpp
// \author mstarch
// \brief  hpp file for SocketIpDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef REF_SOCKETIPDRIVERCFG_HPP
#define REF_SOCKETIPDRIVERCFG_HPP

#define KEEPALIVE_DATA "sitting well" // Ground-based keepalive packet for filtering out packet

enum SocketIpCfg {
    SOCKET_TIMEOUT_SECONDS = 1,       // Seconds component of timeout
    SOCKET_TIMEOUT_MICROSECONDS = 0,  // Milliseconds component of timeout
    SOCKET_SEND_UDP = 1,              // 0 - Send down using TCP, 1 - Send down using UDP
    SOCKET_SEND_FLAGS = 0,            // send, sendto FLAGS argument
    SOCKET_RECV_FLAGS = 0,            // recv FLAGS argument
    RECONNECT_AUTOMATICALLY = 1,      // Attempt to reconnect when a socket closes
    MAX_SEND_ITERATIONS = 0xFFFF,     // Maximum send iterations
    MAX_RECV_BUFFER_SIZE = 2048,      // Maximum and allocation size of the send buffer. TODO: use buffer manager
    PRE_CONNECTION_RETRY_INTERVAL_MS = 1000, // Interval between connection retries before main recv thread starts
    MAX_HOSTNAME_SIZE = 256 // Maximum stored hostname
};



#endif //REF_SOCKETIPDRIVERCFG_HPP
