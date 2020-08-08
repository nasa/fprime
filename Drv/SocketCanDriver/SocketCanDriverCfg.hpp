// ======================================================================
// \title  SocketCanDriverCfg.hpp
// \author csommer
// \brief  hpp file for SocketCanDriver component implementation class
//
// \copyright
// Copyright 2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef REF_SOCKETCANDRIVERCFG_HPP
#define REF_SOCKETCANDRIVERCFG_HPP


// Includes for SocketCAN
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN || TGT_OS_TYPE_RTEMS
    #include <linux/can.h>
#else
    #error OS not supported for SocketCAN Communications
#endif


/* Default (arbitrary) CAN identifier with Extended Frame Format
   The lowest ID value has the highest priority on the bus */
#define SOCKETCAN_DEFAULT_ID      (0x01234567 | CAN_EFF_FLAG)

/* Default CAN mask (accepts all bytes)
   If (Received CAN Frame ID & Mask) == (Filter & Mask)
   then the frame is accepted */
#define SOCKETCAN_DEFAULT_MASK    (0x00000000)

/* Default CAN filter (accepts all bytes) */
#define SOCKETCAN_DEFAULT_FILTER  (0x00000000)

enum SocketCanCfg {
    SOCKETCAN_TIMEOUT_SECONDS = 1,                  // Seconds component of timeout
    SOCKETCAN_TIMEOUT_MICROSECONDS = 0,             // Milliseconds component of timeout
    CAN_RECONNECT_AUTOMATICALLY = 1,                // Attempt to reconnect when a socket closes
    CAN_MAX_SEND_ITERATIONS = 0xFFFF,               // Maximum send iterations
    CAN_MAX_RECV_BUFFER_SIZE = 2048,                // Maximum and allocation size of the send buffer. TODO: use buffer manager
    CAN_PRE_CONNECTION_RETRY_INTERVAL_MS = 1000     // Interval between connection retries before main recv thread starts
};



#endif //REF_SOCKETCANDRIVERCFG_HPP
