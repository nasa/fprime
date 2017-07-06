#ifndef SVC_CMD_SOCKET_PROVIDER_IMPL_HPP
#define SVC_CMD_SOCKET_PROVIDER_IMPL_HPP

#include <Svc/GndIf/GndIfComponentAc.hpp>
#include <Os/Task.hpp>

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
            void openSocket(NATIVE_INT_TYPE port);
            void startSocketTask(I32 priority, U32 port_number, char* hostname);
        private:


            //! Handler for input port fileDownlinkBufferSendIn
            //
            void fileDownlinkBufferSendIn_handler(
                NATIVE_INT_TYPE portNum, /*!< The port number*/
                Fw::Buffer fwBuffer
            );

            static void socketReadTask(void* ptr);
            void downlinkPort_handler(NATIVE_INT_TYPE portNum, Fw::ComBuffer &data, U32 context);
            NATIVE_INT_TYPE m_socketFd; // !< socket file descriptor
            NATIVE_INT_TYPE m_connectionFd; // !< connection file descriptor
            U32 port_number;
            char* hostname;
            Os::Task socketTask;
    };

}

#endif
