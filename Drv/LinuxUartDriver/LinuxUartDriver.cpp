// ======================================================================
// \title  LinuxUartDriverImpl.cpp
// \author tcanham
// \brief  cpp file for LinuxUartDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <unistd.h>
#include <Drv/LinuxUartDriver/LinuxUartDriver.hpp>
#include <Os/TaskString.hpp>

#include "Fw/Types/BasicTypes.hpp"

#include <fcntl.h>
#include <termios.h>
#include <cerrno>

//#include <cstdlib>
//#include <cstdio>
//#define DEBUG_PRINT(x,...) printf(x,##__VA_ARGS__); fflush(stdout)
#define DEBUG_PRINT(x, ...)

namespace Drv {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

LinuxUartDriver ::LinuxUartDriver(const char* const compName)
    : LinuxUartDriverComponentBase(compName), m_fd(-1), m_allocationSize(-1),  m_device("NOT_EXIST"), m_quitReadThread(false) {
}

void LinuxUartDriver ::init(const NATIVE_INT_TYPE instance) {
  LinuxUartDriverComponentBase::init(instance);
}

bool LinuxUartDriver::open(const char* const device,
                           UartBaudRate baud,
                           UartFlowControl fc,
                           UartParity parity,
                           NATIVE_INT_TYPE allocationSize) {
    FW_ASSERT(device != nullptr);
    NATIVE_INT_TYPE fd = -1;
    NATIVE_INT_TYPE stat = -1;
    this->m_allocationSize = allocationSize;

    this->m_device = device;

    DEBUG_PRINT("Opening UART device %s\n", device);

    /*
     The O_NOCTTY flag tells UNIX that this program doesn't want to be the "controlling terminal" for that port. If you
     don't specify this then any input (such as keyboard abort signals and so forth) will affect your process. Programs
     like getty(1M/8) use this feature when starting the login process, but normally a user program does not want this
     behavior.
     */
    fd = ::open(device, O_RDWR | O_NOCTTY);

    if (fd == -1) {
        DEBUG_PRINT("open UART device %s failed.\n", device);
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_OpenError(_arg, this->m_fd, _err);
        return false;
    } else {
        DEBUG_PRINT("Successfully opened UART device %s fd %d\n", device, fd);
    }

    this->m_fd = fd;

    // Configure blocking reads
    struct termios cfg;

    stat = tcgetattr(fd, &cfg);
    if (-1 == stat) {
        DEBUG_PRINT("tcgetattr failed: (%d): %s\n", stat, strerror(errno));
        close(fd);
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_OpenError(_arg, fd, _err);
        return false;
    } else {
        DEBUG_PRINT("tcgetattr passed.\n");
    }

    /*
     If MIN > 0 and TIME = 0, MIN sets the number of characters to receive before the read is satisfied. As TIME is
     zero, the timer is not used.

     If MIN = 0 and TIME > 0, TIME serves as a timeout value. The read will be satisfied if a single character is read,
     or TIME is exceeded (t = TIME *0.1 s). If TIME is exceeded, no character will be returned.

     If MIN > 0 and TIME > 0, TIME serves as an inter-character timer. The read will be satisfied if MIN characters are
     received, or the time between two characters exceeds TIME. The timer is restarted every time a character is
     received and only becomes active after the first character has been received.

     If MIN = 0 and TIME = 0, read will be satisfied immediately. The number of characters currently available, or the
     number of characters requested will be returned. According to Antonino (see contributions), you could issue a
     fcntl(fd, F_SETFL, FNDELAY); before reading to get the same result.
     */
    cfg.c_cc[VMIN] = 0;
    cfg.c_cc[VTIME] = 10;  // 1 sec timeout on no-data

    stat = tcsetattr(fd, TCSANOW, &cfg);
    if (-1 == stat) {
        DEBUG_PRINT("tcsetattr failed: (%d): %s\n", stat, strerror(errno));
        close(fd);
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_OpenError(_arg, fd, _err);
        return false;
    } else {
        DEBUG_PRINT("tcsetattr passed.\n");
    }

    // Set flow control
    if (fc == HW_FLOW) {
        struct termios t;

        int stat = tcgetattr(fd, &t);
        if (-1 == stat) {
            DEBUG_PRINT("tcgetattr UART fd %d failed\n", fd);
            close(fd);
            Fw::LogStringArg _arg = device;
            Fw::LogStringArg _err = strerror(errno);
            this->log_WARNING_HI_OpenError(_arg, fd, _err);
            return false;
        }

        // modify flow control flags
        t.c_cflag |= CRTSCTS;

        stat = tcsetattr(fd, TCSANOW, &t);
        if (-1 == stat) {
            DEBUG_PRINT("tcsetattr UART fd %d failed\n", fd);
            close(fd);
            Fw::LogStringArg _arg = device;
            Fw::LogStringArg _err = strerror(errno);
            this->log_WARNING_HI_OpenError(_arg, fd, _err);
            return false;
        }
    }

    NATIVE_INT_TYPE relayRate = B0;
    switch (baud) {
        case BAUD_9600:
            relayRate = B9600;
            break;
        case BAUD_19200:
            relayRate = B19200;
            break;
        case BAUD_38400:
            relayRate = B38400;
            break;
        case BAUD_57600:
            relayRate = B57600;
            break;
        case BAUD_115K:
            relayRate = B115200;
            break;
        case BAUD_230K:
            relayRate = B230400;
            break;
#if defined TGT_OS_TYPE_LINUX
        case BAUD_460K:
            relayRate = B460800;
            break;
        case BAUD_921K:
            relayRate = B921600;
            break;
#endif
        default:
            FW_ASSERT(0, baud);
            break;
    }

    struct termios newtio;

    stat = tcgetattr(fd, &newtio);
    if (-1 == stat) {
        DEBUG_PRINT("tcgetattr UART fd %d failed\n", fd);
        close(fd);
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_OpenError(_arg, fd, _err);
        return false;
    }

    // CS8 = 8 data bits, CLOCAL = Local line, CREAD = Enable Receiver
    /*
      Even parity (7E1):
      options.c_cflag |= PARENB
      options.c_cflag &= ~PARODD
      options.c_cflag &= ~CSTOPB
      options.c_cflag &= ~CSIZE;
      options.c_cflag |= CS7;
      Odd parity (7O1):
      options.c_cflag |= PARENB
      options.c_cflag |= PARODD
      options.c_cflag &= ~CSTOPB
      options.c_cflag &= ~CSIZE;
      options.c_cflag |= CS7;
     */
    newtio.c_cflag = CS8 | CLOCAL | CREAD;

    switch (parity) {
        case PARITY_ODD:
            newtio.c_cflag |= (PARENB | PARODD);
            break;
        case PARITY_EVEN:
            newtio.c_cflag |= PARENB;
            break;
        case PARITY_NONE:
            newtio.c_cflag &= ~PARENB;
            break;
        default:
            FW_ASSERT(0, parity);
            break;
    }

    // Set baud rate:
    stat = cfsetispeed(&newtio, relayRate);
    if (stat) {
        DEBUG_PRINT("cfsetispeed failed\n");
        close(fd);
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_OpenError(_arg, fd, _err);
        return false;
    }
    stat = cfsetospeed(&newtio, relayRate);
    if (stat) {
        DEBUG_PRINT("cfsetospeed failed\n");
        close(fd);
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_OpenError(_arg, fd, _err);
        return false;
    }

    // Raw output:
    newtio.c_oflag = 0;

    // set input mode (non-canonical, no echo,...)
    newtio.c_lflag = 0;

    newtio.c_iflag = INPCK;

    // Flush old data:
    (void)tcflush(fd, TCIFLUSH);

    // Set attributes:
    stat = tcsetattr(fd, TCSANOW, &newtio);
    if (-1 == stat) {
        DEBUG_PRINT("tcsetattr UART fd %d failed\n", fd);
        close(fd);
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_OpenError(_arg, fd, _err);
        return false;
    }

    // All done!
    Fw::LogStringArg _arg = device;
    this->log_ACTIVITY_HI_PortOpened(_arg);
    this->ready_out(0); // Indicate the driver is connected
    return true;
}

LinuxUartDriver ::~LinuxUartDriver() {
    if (this->m_fd != -1) {
        DEBUG_PRINT("Closing UART device %d\n", this->m_fd);

        (void)close(this->m_fd);
    }
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

Drv::SendStatus LinuxUartDriver ::send_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& serBuffer) {
    Drv::SendStatus status = Drv::SendStatus::SEND_OK;
    if (this->m_fd == -1 || serBuffer.getData() == nullptr || serBuffer.getSize() == 0) {
        status = Drv::SendStatus::SEND_ERROR;
    } else {
        unsigned char *data = serBuffer.getData();
        NATIVE_INT_TYPE xferSize = serBuffer.getSize();

        NATIVE_INT_TYPE stat = ::write(this->m_fd, data, xferSize);

        if (-1 == stat || stat != xferSize) {
          Fw::LogStringArg _arg = this->m_device;
          this->log_WARNING_HI_WriteError(_arg, stat);
          status = Drv::SendStatus::SEND_ERROR;
        }
    }
    // Deallocate when necessary
    if (isConnected_deallocate_OutputPort(0)) {
        deallocate_out(0, serBuffer);
    }
    return status;
}

void LinuxUartDriver ::serialReadTaskEntry(void* ptr) {
    FW_ASSERT(ptr != nullptr);
    Drv::RecvStatus status = RecvStatus::RECV_ERROR;  // added by m.chase 03.06.2017
    LinuxUartDriver* comp = reinterpret_cast<LinuxUartDriver*>(ptr);
    while (!comp->m_quitReadThread) {
        Fw::Buffer buff = comp->allocate_out(0, comp->m_allocationSize);

        // On failed allocation, error and deallocate
        if (buff.getData() == nullptr) {
            Fw::LogStringArg _arg = comp->m_device;
            comp->log_WARNING_HI_NoBuffers(_arg);
            status = RecvStatus::RECV_ERROR;
            comp->recv_out(0, buff, status);
            // to avoid spinning, wait 50 ms
            Os::Task::delay(50);
            continue;
        }

        //          timespec stime;
        //          (void)clock_gettime(CLOCK_REALTIME,&stime);
        //          DEBUG_PRINT("<<< Calling dsp_relay_uart_relay_read() at %d %d\n", stime.tv_sec, stime.tv_nsec);

        int stat = 0;

        // Read until something is received or an error occurs. Only loop when
        // stat == 0 as this is the timeout condition and the read should spin
        while ((stat == 0) && !comp->m_quitReadThread) {
            stat = ::read(comp->m_fd, buff.getData(), buff.getSize());
        }
        buff.setSize(0);

        // On error stat (-1) must mark the read as error
        // On normal stat (>0) pass a recv ok
        // On timeout stat (0) and m_quitReadThread, error to return the buffer
        if (stat == -1) {
            Fw::LogStringArg _arg = comp->m_device;
            comp->log_WARNING_HI_ReadError(_arg, stat);
            status = RecvStatus::RECV_ERROR;
        } else if (stat > 0) {
            buff.setSize(stat);
            status = RecvStatus::RECV_OK;  // added by m.chase 03.06.2017
        } else {
            status = RecvStatus::RECV_ERROR; // Simply to return the buffer
        }
        comp->recv_out(0, buff, status);  // added by m.chase 03.06.2017
    }
}

void LinuxUartDriver ::startReadThread(NATIVE_UINT_TYPE priority,
                                       NATIVE_UINT_TYPE stackSize,
                                       NATIVE_UINT_TYPE cpuAffinity) {
    Os::TaskString task("SerReader");
    Os::Task::TaskStatus stat =
        this->m_readTask.start(task, serialReadTaskEntry, this, priority, stackSize, cpuAffinity);
    FW_ASSERT(stat == Os::Task::TASK_OK, stat);
}

void LinuxUartDriver ::quitReadThread() {
    this->m_quitReadThread = true;
}

Os::Task::TaskStatus LinuxUartDriver ::join(void** value_ptr) {
    return m_readTask.join(value_ptr);
}

}  // end namespace Drv
