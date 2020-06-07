/*
 * SocketIpTypes.hpp
 *
 *  Created on: Jun 2, 2020
 *      Author: tcanham
 */

#ifndef DRV_SOCKETIPDRIVER_SOCKETIPDRIVERTYPES_HPP_
#define DRV_SOCKETIPDRIVER_SOCKETIPDRIVERTYPES_HPP_

namespace Drv {


    // Put the definitions here so that the
    // driver and helper can share

    enum SocketProtocol {
        SOCK_PROT_TCP, //!< Send using TCP
        SOCK_PROT_UDP //!< Send using UDP
    };

    enum SocketIpStatus {
        SOCK_SUCCESS = 0,
        SOCK_FAILED_TO_GET_SOCKET = -1,
        SOCK_FAILED_TO_GET_HOST_IP = -2,
        SOCK_INVALID_IP_ADDRESS = -3,
        SOCK_FAILED_TO_CONNECT = -4,
        SOCK_FAILED_TO_SET_SOCKET_OPTIONS = -5,
        SOCK_INTERRUPTED_TRY_AGAIN = -6,
        SOCK_READ_ERROR = -7,
        SOCK_READ_DISCONNECTED = -8
    };


}



#endif /* DRV_SOCKETIPDRIVER_SOCKETIPDRIVERTYPES_HPP_ */
