#include <Svc/SocketGndIf/SvcSocketGndIfImpl.hpp>
#include <Fw/Com/ComPacket.hpp>

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
#elif defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN || TGT_OS_TYPE_RTAI
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
    SocketGndIfImpl::SocketGndIfImpl(const char* name) :
        GndIfComponentBase(name),m_socketFd(-1),m_connectionFd(-1),port_number(0),hostname(NULL) {
    }
#else
    SocketGndIfImpl::SocketGndIfImpl() :
        GndIfComponentBase(),m_socketFd(-1),m_connectionFd(-1) {
    }
#endif
    void SocketGndIfImpl::init(NATIVE_INT_TYPE instance) {
        GndIfComponentBase::init(instance);
    }

    SocketGndIfImpl::~SocketGndIfImpl() {
    }

    void SocketGndIfImpl::startSocketTask(I32 priority, U32 port_number, char* hostname) {
        Fw::EightyCharString name("ScktRead");
        this->port_number = port_number;
        this->hostname = hostname;

        if (this->port_number == 0){
        	return;
        }
        else {
                // Try to open socket before spawning read task:
                log_WARNING_LO_NoConnectionToServer(port_number);
               	openSocket(port_number);

                // Spawn read task:
        	Os::Task::TaskStatus stat = this->socketTask.start(name,0,priority,10*1024,SocketGndIfImpl::socketReadTask, (void*) this);
        	FW_ASSERT(Os::Task::TASK_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
        }
    }

    void SocketGndIfImpl::openSocket(NATIVE_INT_TYPE port) {
        // set up port connection
        struct sockaddr_in servaddr; // Internet socket address struct
        int sockAddrSize;
        char ip[100];
        char buf[256];

        if ((this->m_socketFd = socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
            (void) printf("Socket error: %s\n",strerror(errno));
            return;
        }

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        // set socket write to timeout after 1 sec
        if (setsockopt (this->m_socketFd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
                        sizeof(timeout)) < 0) {
            (void)printf("setsockopt error: %s\n",strerror(errno));
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

        strncpy(buf, "Register FSW\n", sizeof(buf));
#ifdef TGT_OS_TYPE_VXWORKS
        if (socketWrite(this->m_socketFd, (U8*)buf, strlen(buf)) < 0) { // TKC: VxWorks doesn't have strnlen.
#else
        if (socketWrite(this->m_socketFd, (U8*)buf, strnlen(buf, 13)) < 0) {
#endif
            (void) printf("write error on port %d \n", port);
            return;
        }
        this->log_ACTIVITY_HI_ConnectedToServer(port);
        (void) printf("Sent register FSW string to ground\n");
        //m_connectionFd is set so m_socketFd can register FSW before other tasks try to write data to ground
        this->m_connectionFd = this->m_socketFd;
    }

    void SocketGndIfImpl::socketReadTask(void* ptr) {
        FW_ASSERT(ptr);
        bool acceptConnections;
        uint32_t packetDelimiter;
        // Gotta love VxWorks...
        uint32_t packetSize;
        uint32_t packetDesc;
        U8 buf[FW_COM_BUFFER_MAX_SIZE];
        ssize_t bytesRead;

        // cast pointer to component type
        SocketGndIfImpl* comp = (SocketGndIfImpl*) ptr;

        while (comp->m_socketFd == -1){
        	comp->openSocket(comp->port_number);
        	(void)Os::Task::delay(1000);
        }

        acceptConnections = true;

        // loop until magic "kill" packet
        while (acceptConnections) {
//            struct sockaddr them;
//#ifdef __VXWORKS__ // Gotta love VxWorks...
//            int theirSize
//#else
//            socklen_t theirSize
//#endif
//            = sizeof(them);

            // read packets
            while (true) {
                // first, read packet delimiter
                bytesRead = socketRead(comp->m_socketFd,(U8*)&packetDelimiter,sizeof(packetDelimiter));
                if( -1 == bytesRead ) {
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
                bytesRead = socketRead(comp->m_connectionFd,(U8*)&packetSize,sizeof(packetSize));
                if( -1 == bytesRead ) {
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


                // get packet description
                bytesRead = socketRead(comp->m_socketFd,(U8*)&packetDesc,sizeof(packetDesc));
                packetDesc = ntohl(packetDesc);


                switch(packetDesc) {
                    case Fw::ComPacket::FW_PACKET_COMMAND:

                        // check size of command
                        if (packetSize > FW_COM_BUFFER_MAX_SIZE) {
                            (void) printf("Packet to large! :%d\n",packetSize);
                            // might as well wait for the next packet
                            break;
                        }

                        // read cmd packet minus description
                        bytesRead = socketRead(comp->m_socketFd,(U8*)buf+sizeof(packetDesc),packetSize-sizeof(packetDesc));
                        if (-1 == bytesRead) {
                            (void) printf("Size read error: %s\n",strerror(errno));
                            break;
                        }

                        if(0 == bytesRead) {
                            comp->m_connectionFd = -1;
                            break;
                        }

                        // Add back description
                        bytesRead = bytesRead + sizeof(packetDesc);

                        buf[3] = packetDesc & 0xff;
                        buf[2] = (packetDesc & 0xff00) >> 8;
                        buf[1] = (packetDesc & 0xff0000) >> 16;
                        buf[0] = (packetDesc & 0xff000000) >> 24;

                        // check read size
                        if (bytesRead != (ssize_t)packetSize) {
                            (void) printf("Read size mismatch: A: %ld E: %d\n",(long int)bytesRead,packetSize);
                        }


                        if (comp->isConnected_uplinkPort_OutputPort(0)) {
                             Fw::ComBuffer cmdBuffer(buf, bytesRead);
                             comp->uplinkPort_out(0,cmdBuffer,0);
                        }
                        break;

                    case Fw::ComPacket::FW_PACKET_FILE:
                    {

                        // Get Buffer
                        Fw::Buffer packet_buffer = comp->fileUplinkBufferGet_out(0, packetSize - sizeof(packetDesc));
                        U8* data_ptr = (U8*)packet_buffer.getdata();


                        // Read file packet minus description
                        bytesRead = socketRead(comp->m_socketFd, data_ptr, packetSize - sizeof(packetDesc));
                        if (-1 == bytesRead) {
                            (void) printf("Size read error: %s\n",strerror(errno));
                            break;
                        }

                        // for(uint32_t i =0; i < bytesRead; i++){
                        //     (void) printf("IN_DATA:%02x\n", data_ptr[i]);
                        // }

                        if (comp->isConnected_fileUplinkBufferSendOut_OutputPort(0)) {
                            comp->fileUplinkBufferSendOut_out(0, packet_buffer);
                        }

                    }
                        break;
                    default:
                        FW_ASSERT(0);
                }



            } // while not done with packets
            close(comp->m_socketFd);
            comp->m_connectionFd = -1;

            //try to re-establish connection with server
            if (bytesRead == 0) {
                comp->log_WARNING_LO_LostConnectionToServer(comp->port_number);
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


    void SocketGndIfImpl::fileDownlinkBufferSendIn_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Buffer fwBuffer
    ) {

        char buf[256];
        U32 header_size = 9;
        U32 desc        = 3; // File Desc
        U32 packet_size = 0;
        U32 buffer_size = fwBuffer.getsize();
        U32 net_buffer_size = htonl(buffer_size+4);

        //Write message header
        strncpy(buf, "A5A5 GUI ", sizeof(buf));
        packet_size += header_size;

        memmove(buf + packet_size, &net_buffer_size, sizeof(net_buffer_size));
        packet_size += sizeof(buffer_size);

        desc = htonl(desc);
        memmove(buf + packet_size, &desc, sizeof(desc));
        packet_size += sizeof(desc);

        //Send msg header
        (void)socketWrite(this->m_connectionFd,(U8*)buf, packet_size);

        (void)socketWrite(this->m_connectionFd,(U8*)fwBuffer.getdata(), buffer_size);
        //printf("PACKET BYTES SENT: %d\n", bytes_sent);

        // for(uint32_t i = 0; i < bytes_sent; i++){
        //     printf("%02x\n",((U8*)fwBuffer.getdata())[i]);
        // }
        this->fileDownlinkBufferSendOut_out(0,fwBuffer);

    }


    void SocketGndIfImpl::downlinkPort_handler(NATIVE_INT_TYPE portNum, Fw::ComBuffer &data, U32 context) {
        char buf[256];
        U32 header_size;
        U32 data_size;
        I32 data_net_size;
        I32 bytes_sent;

        //this is the size of "A5A5 GUI "
        header_size = 9;
        data_size = data.getBuffLength();
        data_net_size = htonl(data.getBuffLength());
        // check to see if someone is connected
        if (this->m_connectionFd != -1) {
            //(void) printf("Trying to send %ld bytes to ground.\n",data.getBuffLength() + header_size + sizeof(data_size));
            strncpy(buf, "A5A5 GUI ", sizeof(buf));
            memmove(buf + header_size, &data_net_size, sizeof(data_net_size));
            memmove(buf + header_size + sizeof(data_size), (char *)data.getBuffAddr(), data_size);

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
    }
}
