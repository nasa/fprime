#ifndef SVC_BUFF_GND_SOCK_IF_IMPL_HPP
#define SVC_BUFF_GND_SOCK_IF_IMPL_HPP

#include <Svc/BuffGndSockIf/BuffGndSockIfComponentAc.hpp>
#include <Os/Task.hpp>

namespace Svc {

    class BuffGndSockIfImpl : public BuffGndSockIfComponentBase {
        public:
        
#if FW_OBJECT_NAMES == 1        
            BuffGndSockIfImpl(const char* name);
#else
            BuffGndSockIfImpl();
#endif
            void init(void);
            ~BuffGndSockIfImpl();
            void openSocket(NATIVE_INT_TYPE port);
            void startSocketTask(I32 priority, U32 port_number, char* hostname);
        private:
            static void socketReadTask(void* ptr);
            void downlink_handler(NATIVE_INT_TYPE portNum, Fw::Buffer fwBuffer);
            NATIVE_INT_TYPE m_socketFd; // !< socket file descriptor
            NATIVE_INT_TYPE m_connectionFd; // !< connection file descriptor
            U32 port_number;
            char* hostname;
            Os::Task socketTask;
    };

}

#endif
