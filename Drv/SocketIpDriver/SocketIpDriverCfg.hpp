// ======================================================================
// \title  SocketIpDriverComponentImpl.hpp
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

enum SocketIpCfg {
    SOCKET_TIMEOUT_SECONDS = 1,
    SOCKET_TIMEOUT_MILLI_SECONDS = 0,
    SOCKET_SEND_UDP = 1,
    SOCKET_SEND_FLAGS = 0,
    SOCKET_RECV_FLAGS = 0,
    MAX_SEND_ITERATIONS = 0xFFFF,
    RECV_BUFFER_MAX_SIZE = 2048,
    PRE_CONNECTION_RETRY_INTERVAL_MS = 1000
};



#endif //REF_SOCKETIPDRIVERCFG_HPP
