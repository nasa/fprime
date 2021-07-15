/*
 * SocketHelper.hpp
 *
 *  Created on: May 28, 2020
 *      Author: tcanham
 */

#ifndef DRV_SOCKETIPDRIVER_SOCKETHELPER_HPP_
#define DRV_SOCKETIPDRIVER_SOCKETHELPER_HPP_

#include <Fw/Types/BasicTypes.hpp>
#include <Drv/SocketIpDriver/SocketIpDriverTypes.hpp>
#include <SocketIpDriverCfg.hpp>

namespace Drv {

    // forward declaration for pointer
    struct SocketState;

    class SocketHelper {
        public:

            SocketHelper();
            virtual ~SocketHelper();
            SocketIpStatus configure(
                    const char* hostname,
                    const U16 port,
                    const bool send_udp,
                    const U32 timeout_seconds,
                    const U32 timeout_microseconds
                    );
            bool isOpened();
            SocketIpStatus open();
            void send(U8* data, const U32 size); //Forwards to sendto, which on some OSes requires a non-const data pointer
            SocketIpStatus recv(U8* data, I32 &size);
            void close();

        PRIVATE:

            SocketIpStatus openProtocol(NATIVE_INT_TYPE protocol, bool isInput = true);

            SocketState* m_state;
            NATIVE_INT_TYPE m_socketInFd;  //!< Input file descriptor, always TCP
            NATIVE_INT_TYPE m_socketOutFd; //!< Output file descriptor, always UDP
            bool m_sendUdp;
            U32 m_timeoutSeconds;
            U32 m_timeoutMicroseconds;
            char m_hostname[MAX_HOSTNAME_SIZE]; //!< Hostname to supply
            U16 m_port;                    //!< IP address port used

    };

}

#endif /* DRV_SOCKETIPDRIVER_SOCKETHELPER_HPP_ */
