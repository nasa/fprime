#include <Svc/BuffGndSockIf/BuffGndSockIfImpl.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/EightyCharString.hpp>
#include <Os/Task.hpp>

#ifdef TGT_OS_TYPE_VXWORKS
#include <hostLib.h>
#include <ioLib.h>
#include <socket.h>
#include <vxWorks.h>
#include <sockLib.h>
#include <inetLib.h>
#include <fioLib.h>
#include <taskLib.h>
#include <sysLib.h>
#elif defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#define ERROR (-1)
#else
#error Unsupported OS!
#endif

namespace Svc {

    /////////////////////////////////////////////////////////////////////
    // Helper functions
    /////////////////////////////////////////////////////////////////////
    
    namespace {

        NATIVE_INT_TYPE socketWrite(NATIVE_INT_TYPE fd, U8* buf, U32 size) {
            NATIVE_INT_TYPE total=0;
            while(size > 0) {
                int bytesWritten = write(fd, (char*)buf, size);
                if (bytesWritten == -1) {
                  if (errno == EINTR) continue;
                  return (total == 0) ? -1 : total;
                }
                buf += bytesWritten;
                size -= bytesWritten;
                total += bytesWritten;
            }
            return total;
        }

        NATIVE_INT_TYPE socketRead(NATIVE_INT_TYPE fd, U8* buf, U32 size) {
            NATIVE_INT_TYPE total=0;
            while(size > 0) {
                NATIVE_INT_TYPE bytesRead = read(fd, (char*)buf, size);
                if (bytesRead == -1) {
                  if (errno == EINTR) continue;
                  return (total == 0) ? -1 : total;
                }
                buf += bytesRead;
                size -= bytesRead;
                total += bytesRead;
            }
            return total;
        }
    }

    /////////////////////////////////////////////////////////////////////
    // Class implementation
    /////////////////////////////////////////////////////////////////////

#if FW_OBJECT_NAMES == 1
    BuffGndSockIfImpl::BuffGndSockIfImpl(const char* name) :
        BuffGndSockIfComponentBase(name),m_socketFd(-1),m_connectionFd(-1),port_number(0),hostname(NULL) {
    }
#else
    BuffGndSockIfImpl::BuffGndSockIfImpl() :
        BuffGndSockIfComponentBase(),m_socketFd(-1),m_connectionFd(-1) {
    }
#endif
    void BuffGndSockIfImpl::init(void) {
        BuffGndSockIfComponentBase::init();
    }
    
    BuffGndSockIfImpl::~BuffGndSockIfImpl() {

    }

    void BuffGndSockIfImpl::startSocketTask(I32 priority, U32 port_number, char* hostname) {
        Fw::EightyCharString name("ScktRead");
        this->port_number = port_number;
        this->hostname = hostname;

        if (this->port_number == 0){
        	return;
        }
        else {
                // Try to open socket before spawning read task:
                log_WARNING_LO_BuffGndSockIf_NoConnectionToServer(port_number);
               	openSocket(port_number);

        	Os::Task::TaskStatus stat = this->socketTask.start(name,0,priority,10*1024,BuffGndSockIfImpl::socketReadTask, (void*) this);
        	FW_ASSERT(Os::Task::TASK_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
        }
    }

    void BuffGndSockIfImpl::openSocket(NATIVE_INT_TYPE port) {
        // set up port connection
        struct sockaddr_in servaddr; // Internet socket address struct
        int sockAddrSize;
        char ip[100];
        char buf[256];

        if ((this->m_socketFd = socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
            (void) printf("Socket error: %s\n",strerror(errno));
            return;
        }

        // Fill in data structure with server information
        sockAddrSize = sizeof(struct sockaddr_in);
        memset((char*) &servaddr, 0, sizeof(servaddr)); // set the entire stucture to zero
        servaddr.sin_family = AF_INET; // set address family to AF_INET (2)
#ifdef TGT_OS_TYPE_VXWORKS
        servaddr.sin_len = (u_char) sockAddrSize;
#endif
        servaddr.sin_port = htons(port);

        if (this->hostname == NULL){
        	// Use localhost
            strncpy(ip, "127.0.0.1", sizeof(ip));
        } else {

        #ifdef TGT_OS_TYPE_VXWORKS
        	strncpy(ip, this->hostname, sizeof(ip));
        }
        #else
            struct hostent *he;
            struct in_addr **addr_list;
            if ((he = gethostbyname(this->hostname)) == NULL) {
                (void)printf("Unable to get hostname\n");
                return;
            }

            addr_list = (struct in_addr **) he->h_addr_list;
            strncpy(ip, inet_ntoa(*addr_list[0]), sizeof(ip));
            // null terminate
            ip[sizeof(ip)-1] = 0;
        }
        #endif

        servaddr.sin_addr.s_addr = inet_addr(ip);
        if (connect(this->m_socketFd, (struct sockaddr *) &servaddr, sockAddrSize) < 0) {
            this->m_socketFd = -1;
            return;
        }

        strncpy(buf, "Register ISF\n", sizeof(buf));
#ifdef TGT_OS_TYPE_VXWORKS
        if (socketWrite(this->m_socketFd, (U8*)buf, strlen(buf)) < 0) { // TKC: VxWorks doesn't have strnlen.
#else
        if (socketWrite(this->m_socketFd, (U8*)buf, strnlen(buf, 13)) < 0) {
#endif
            (void) printf("write error on port %d \n", port);
            return;
        }
        this->log_ACTIVITY_HI_BuffGndSockIf_ConnectedToServer(port);
        (void) printf("Sent register ISF string to ground\n");
        //m_connectionFd is set so m_socketFd can register ISF before other tasks try to write data to ground
        this->m_connectionFd = this->m_socketFd;
    }

    void BuffGndSockIfImpl::socketReadTask(void* ptr) {
        FW_ASSERT(ptr);
        bool acceptConnections;
        uint32_t packetDelimiter;
        // Gotta love VxWorks...
        uint32_t packetSize;
        ssize_t bytesRead;

        // cast pointer to component type
        BuffGndSockIfImpl* comp = (BuffGndSockIfImpl*) ptr;

        while (comp->m_socketFd == -1){
        	comp->openSocket(comp->port_number);
        	(void)Os::Task::delay(1000);
        }

        acceptConnections = true;

        // loop until magic "kill" packet
        while (acceptConnections) {

            // read packets
            while (true) {
                // first, read packet delimiter
                bytesRead = socketRead(comp->m_socketFd,(U8*)&packetDelimiter,sizeof(packetDelimiter));
                if (-1 == bytesRead) {
                    (void) printf("Delim read error: %s",strerror(errno));
                    break;
                }

                if(0 == bytesRead) {
                	comp->m_connectionFd = -1;
                	break;
                }

                if (bytesRead != sizeof(packetDelimiter)) {
                    (void) printf("Didn't get right pd size: %ld\n",(long int)bytesRead);
                }
                // correct for network order
                packetDelimiter = ntohl(packetDelimiter);

                // if magic number to quit, exit loop
                if (packetDelimiter == 0xA5A5A5A5) {
                    (void) printf("packetDelimiter = 0x%x\n", packetDelimiter);
                    //break;
                } else if (packetDelimiter != 0x5A5A5A5A) {
                    (void) printf("Unexpected delimiter 0x%08X\n",packetDelimiter);
                    // just keep reading until a delimiter is found
                    continue;
                }

                // now read packet size
                bytesRead = socketRead(comp->m_socketFd,(U8*)&packetSize,sizeof(packetSize));
                if (-1 == bytesRead) {
                    (void) printf("Size read error: %s",strerror(errno));
                    break;
                }

                if(0 == bytesRead) {
                	comp->m_connectionFd = -1;
                	break;
                }

                if (bytesRead != sizeof(packetSize)) {
                    (void) printf("Didn't get right ps size!\n");
                }

                // correct for network order
                packetSize = ntohl(packetSize);

                Fw::Buffer fwBuffer;
                fwBuffer = comp->bufferGet_out(0, packetSize);

                // read packet
                bytesRead = socketRead(comp->m_socketFd,(U8*)fwBuffer.getdata(),packetSize);
                if (-1 == bytesRead) {
                    (void) printf("Size read error: %s\n",strerror(errno));
                    break;
                }

                if(0 == bytesRead) {
                	comp->m_connectionFd = -1;
                	break;
                }

                // check read size
                if (bytesRead != (ssize_t)packetSize) {
                    (void) printf("Read size mismatch: A: %ld E: %d\n",(long int)bytesRead,packetSize);
                }

                if (comp->isConnected_uplink_OutputPort(0)) {
                    comp->uplink_out(0, fwBuffer);
                }

            } // while not done with packets
            close(comp->m_socketFd);
            comp->m_connectionFd = -1;

            //try to re-establish connection with server
            if (bytesRead == 0) {
                comp->log_WARNING_LO_BuffGndSockIf_LostConnectionToServer(comp->port_number);
                while (comp->m_connectionFd == -1){
                    Os::Task::delay(5000);
                	comp->openSocket(comp->port_number);
                }
            }
            else {
            	break;
            }
        }

    }

    void BuffGndSockIfImpl::downlink_handler(NATIVE_INT_TYPE portNum, Fw::Buffer fwBuffer) {
        U32 header_size;
        U32 data_size;
        I32 data_net_size;
        I32 bytes_sent;

        //this is the size of "A5A5 GUI "
        header_size = 9;
        data_size = fwBuffer.getsize();
        data_net_size = htonl(fwBuffer.getsize());

        //create buffer for socket write data
        char buf[header_size + sizeof(data_size) + data_size];

        // check to see if someone is connected
        if (this->m_connectionFd != -1) {
            //(void) printf("Trying to send %ld bytes to ground.\n",data.getBuffLength() + header_size + sizeof(data_size));
            strncpy(buf, "A5A5 GUI ", sizeof(buf));
            memmove(buf + header_size, &data_net_size, sizeof(data_net_size));
            memmove(buf + header_size + sizeof(data_size), (char *)fwBuffer.getdata(), data_size);

            //TODO: need to make the write call more robust (continue writing if first write didn't write everything)
            bytes_sent = socketWrite(this->m_connectionFd,(U8*)buf, header_size + data_size + sizeof(data_size));
            if (bytes_sent < 0) {
            	(void) printf("write error on port %d \n", this->port_number);
            	return;
            }
            else if (bytes_sent != (I32)(header_size + data_size + sizeof(data_size))) {
                (void) printf("Not all data sent. E: %lu A: %d\n", (long unsigned int)(header_size + data_size + sizeof(data_size)), bytes_sent);
            } else {
//                (void) printf("Sent %d bytes to ground.\n",bytes_sent);
            }
        }

        //return buffer from the downlink port
        bufferReturn_out(0, fwBuffer);
    }
}

