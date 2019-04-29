#ifndef SVC_CMD_SOCKET_PROVIDER_IMPL_HPP
#define SVC_CMD_SOCKET_PROVIDER_IMPL_HPP

#include <Svc/GndIf/GndIfComponentAc.hpp>
#include <Os/Task.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>

namespace Svc {

    class SocketGndIfImpl : public GndIfComponentBase {
        public:
        
#if FW_OBJECT_NAMES == 1        
            SocketGndIfImpl(const char* name);
#else
            SocketGndIfImpl();
#endif
            void init(NATIVE_INT_TYPE instance);
            ~SocketGndIfImpl();

            enum DownlinkProt {
                SEND_UDP,
                SEND_TCP
            };

            void setSocketTaskProperties(I32 priority, NATIVE_INT_TYPE stackSize, U32 portNumber, char* hostname, DownlinkProt prot = SEND_UDP, NATIVE_INT_TYPE cpuAffinity = -1);

            void startSocketTask(NATIVE_INT_TYPE priority, NATIVE_INT_TYPE stackSize, U32 port_number, char* hostname,  DownlinkProt prot = SEND_UDP, NATIVE_INT_TYPE cpuAffinity = -1);
            void setUseDefaultHeader(bool useDefault);
        private:

            void openSocket(NATIVE_INT_TYPE port, DownlinkProt prot = SEND_UDP);

            static const U32 PKT_DELIM = 0x5A5A5A5A;

            //! Handler for input port fileDownlinkBufferSendIn
            //
            void fileDownlinkBufferSendIn_handler(
                NATIVE_INT_TYPE portNum, /*!< The port number*/
                Fw::Buffer &fwBuffer
            );

            //! Implementation for GNDIF_ENABLE_INTERFACE command handler
            //! Enable the interface for ground communications
            void GNDIF_ENABLE_INTERFACE_cmdHandler(
                const FwOpcodeType opCode, /*!< The opcode*/
                const U32 cmdSeq /*!< The command sequence number*/
            );

            static void socketReadTask(void* ptr);
            void downlinkPort_handler(NATIVE_INT_TYPE portNum, Fw::ComBuffer &data, U32 context);
            Svc::ConnectionStatus isConnected_handler(NATIVE_INT_TYPE portNum);

            NATIVE_INT_TYPE m_socketFd; // !< socket file descriptor
            NATIVE_INT_TYPE m_connectionFd; // !< connection file descriptor
            NATIVE_INT_TYPE m_udpFd; // !< udp socket if used
            NATIVE_INT_TYPE m_udpConnectionFd; // !< connection file descriptor
            struct sockaddr_in m_servAddr; // !< UDP server

            I32 m_priority;
            NATIVE_INT_TYPE m_stackSize;
            U32 port_number;
            char* hostname;
            NATIVE_INT_TYPE m_cpuAffinity;
            Os::Task socketTask;
            bool useDefaultHeader; // Use the default header for downlink, ie "A5A5 GUI "
            DownlinkProt m_prot; // is downlink TCP or UDP

            bool m_portConfigured;
    };

}

#endif
