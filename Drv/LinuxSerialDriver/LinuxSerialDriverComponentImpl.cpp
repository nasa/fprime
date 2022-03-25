// ======================================================================
// \title  LinuxSerialDriverImpl.cpp
// \author tcanham
// \brief  cpp file for LinuxSerialDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Drv/LinuxSerialDriver/LinuxSerialDriverComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include <Os/TaskString.hpp>
#include <cstdlib>
#include <unistd.h>
#include <ctime>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <cstdio>
#include <cerrno>

//#define DEBUG_PRINT(x,...) printf(x,##__VA_ARGS__); fflush(stdout)
#define DEBUG_PRINT(x,...)

namespace Drv {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  bool LinuxSerialDriverComponentImpl::open(const char* const device, UartBaudRate baud, UartFlowControl fc, UartParity parity, bool block) {

      /*
       Their config:

       2-wire UART on BLSP2 is accessible via /dev/ttyHS0
       2-wire UART on BLSP9 is accessible via /dev/ttyHS3
       4-wire UART on BLSP6 is accessible via /dev/ttyHS2

       Our config:
       tty-1 bam-2 2-wire
       tty-2 bam-6 4-wire
       tty-3 bam-9 4-wire
       */

      // TODO remove printf

      NATIVE_INT_TYPE fd;
      NATIVE_INT_TYPE stat;

      this->m_device = device;

      DEBUG_PRINT("Opening UART device %s\n", device);


      // TODO can't use O_NDELAY and block (it is same as O_NONBLOCK), so removing NDELAY for now
      /*
       The O_NOCTTY flag tells UNIX that this program doesn't want to be the "controlling terminal" for that port. If you don't specify this then any input (such as keyboard abort signals and so forth) will affect your process. Programs like getty(1M/8) use this feature when starting the login process, but normally a user program does not want this behavior.

       The O_NDELAY flag tells UNIX that this program doesn't care what state the DCD signal line is in - whether the other end of the port is up and running. If you do not specify this flag, your process will be put to sleep until the DCD signal line is the space voltage.
       */
      fd = ::open(device, O_RDWR | O_NOCTTY); // | O_NDELAY);
      //fd = open(device, O_RDWR | O_NONBLOCK | O_SYNC);

      if (fd == -1) {
          DEBUG_PRINT("open UART device %s failed.\n", device);
          Fw::LogStringArg _arg = device;
          Fw::LogStringArg _err = strerror(errno);
          this->log_WARNING_HI_DR_OpenError(_arg,this->m_fd,_err);
          return false;
      } else {
          DEBUG_PRINT("Successfully opened UART device %s fd %d\n", device, fd);
      }

      this->m_fd = fd;

      // Configure blocking reads
      struct termios cfg;

      stat = tcgetattr(fd,&cfg);
      if (-1 == stat) {
          DEBUG_PRINT("tcgetattr failed: (%d): %s\n",stat,strerror(errno));
          close(fd);
          Fw::LogStringArg _arg = device;
          Fw::LogStringArg _err = strerror(errno);
          this->log_WARNING_HI_DR_OpenError(_arg,fd,_err);
          return false;
      } else {
          DEBUG_PRINT("tcgetattr passed.\n");
      }

      /*
       If MIN > 0 and TIME = 0, MIN sets the number of characters to receive before the read is satisfied. As TIME is zero, the timer is not used.

       If MIN = 0 and TIME > 0, TIME serves as a timeout value. The read will be satisfied if a single character is read, or TIME is exceeded (t = TIME *0.1 s). If TIME is exceeded, no character will be returned.

       If MIN > 0 and TIME > 0, TIME serves as an inter-character timer. The read will be satisfied if MIN characters are received, or the time between two characters exceeds TIME. The timer is restarted every time a character is received and only becomes active after the first character has been received.

       If MIN = 0 and TIME = 0, read will be satisfied immediately. The number of characters currently available, or the number of characters requested will be returned. According to Antonino (see contributions), you could issue a fcntl(fd, F_SETFL, FNDELAY); before reading to get the same result.
       */
      // wait for at least 1 byte for 1 second
      // TODO VMIN should be 0 when using VTIME, and then it would give the timeout behavior Tim wants
      cfg.c_cc[VMIN] = 0; // TODO back to 0
      cfg.c_cc[VTIME] = 10; // 1 sec, TODO back to 10

      stat = tcsetattr(fd,TCSANOW,&cfg);
      if (-1 == stat) {
          DEBUG_PRINT("tcsetattr failed: (%d): %s\n",stat,strerror(errno));
          close(fd);
          Fw::LogStringArg _arg = device;
          Fw::LogStringArg _err = strerror(errno);
          this->log_WARNING_HI_DR_OpenError(_arg,fd,_err);
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
              this->log_WARNING_HI_DR_OpenError(_arg,fd,_err);
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
              this->log_WARNING_HI_DR_OpenError(_arg,fd,_err);
              return false;
          }
      }

      NATIVE_INT_TYPE relayRate = B0;
      switch (baud) {
          // TODO add more as needed
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
              FW_ASSERT(0,baud);
              break;
      }

      struct termios newtio;

      stat = tcgetattr(fd, &newtio);
      if (-1 == stat) {
          DEBUG_PRINT("tcgetattr UART fd %d failed\n", fd);
          close(fd);
          Fw::LogStringArg _arg = device;
          Fw::LogStringArg _err = strerror(errno);
          this->log_WARNING_HI_DR_OpenError(_arg,fd,_err);
          return false;
      }

      // CS8 = 8 data bits, CLOCAL = Local line, CREAD = Enable Receiver
      // TODO PARENB for parity bit
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
              newtio.c_cflag |= (PARENB | PARODD) ;
              break;
          case PARITY_EVEN:
              newtio.c_cflag |= PARENB ;
              break;
          case PARITY_NONE:
              newtio.c_cflag &= ~PARENB ;
              break;
          default:
              FW_ASSERT(0,parity);
              break;
      }

      // Set baud rate:
      stat = cfsetispeed(&newtio, relayRate);
      if (stat) {
          DEBUG_PRINT("cfsetispeed failed\n");
          close(fd);
          Fw::LogStringArg _arg = device;
          Fw::LogStringArg _err = strerror(errno);
          this->log_WARNING_HI_DR_OpenError(_arg,fd,_err);
          return false;
      }
      stat = cfsetospeed(&newtio, relayRate);
      if (stat) {
          DEBUG_PRINT("cfsetospeed failed\n");
          close(fd);
          Fw::LogStringArg _arg = device;
          Fw::LogStringArg _err = strerror(errno);
          this->log_WARNING_HI_DR_OpenError(_arg,fd,_err);
          return false;
      }

      // Raw output:
      newtio.c_oflag = 0;

      // set input mode (non-canonical, no echo,...)
      newtio.c_lflag = 0;

      // TODO if parity, then do input parity too
      //options.c_iflag |=INPCK;
      newtio.c_iflag = INPCK;

      // Flush old data:
      (void) tcflush(fd, TCIFLUSH);

      // Set attributes:
      stat = tcsetattr(fd,TCSANOW,&newtio);
      if (-1 == stat) {
          DEBUG_PRINT("tcsetattr UART fd %d failed\n", fd);
          close(fd);
          Fw::LogStringArg _arg = device;
          Fw::LogStringArg _err = strerror(errno);
          this->log_WARNING_HI_DR_OpenError(_arg,fd,_err);
          return false;
      }

      // All done!
      Fw::LogStringArg _arg = device;
      this->log_ACTIVITY_HI_DR_PortOpened(_arg);
      return true;
  }

  LinuxSerialDriverComponentImpl ::
    ~LinuxSerialDriverComponentImpl()
  {
      if (this->m_fd != -1) {
          DEBUG_PRINT("Closing UART device %d\n", this->m_fd);

          (void) close(this->m_fd);
      }
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void LinuxSerialDriverComponentImpl ::
    serialSend_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &serBuffer
    )
  {
      if (this->m_fd == -1) {
          return;
      }

      unsigned char* data = serBuffer.getData();
      NATIVE_INT_TYPE xferSize = serBuffer.getSize();

      NATIVE_INT_TYPE stat = ::write(this->m_fd,data,xferSize);

      // TODO no need to delay for writes b/c the write blocks
      // not sure if it will block until everything is transmitted, but seems to

      if (-1 == stat || stat != xferSize) {
          Fw::LogStringArg _arg = this->m_device;
          this->log_WARNING_HI_DR_WriteError(_arg,stat);
      }
  }

  void LinuxSerialDriverComponentImpl ::
    serialReadTaskEntry(void * ptr) {

      Drv::SerialReadStatus serReadStat; // added by m.chase 03.06.2017

      LinuxSerialDriverComponentImpl* comp = static_cast<LinuxSerialDriverComponentImpl*>(ptr);

      Fw::Buffer buff;

      while (true) {
          // wait for data
          int sizeRead = 0;

          // find open buffer

          comp->m_readBuffMutex.lock();
          // search for open entry
          NATIVE_INT_TYPE entryFound = false;
          for (NATIVE_INT_TYPE entry = 0; entry < DR_MAX_NUM_BUFFERS; entry++) {
              if (comp->m_buffSet[entry].available) {
                  comp->m_buffSet[entry].available = false;
                  buff = comp->m_buffSet[entry].readBuffer;
                  entryFound = true;
                  break;
              }
          }
          comp->m_readBuffMutex.unLock();

          if (not entryFound) {
              Fw::LogStringArg _arg = comp->m_device;
              comp->log_WARNING_HI_DR_NoBuffers(_arg);
              serReadStat = SerialReadStatus::SER_NO_BUFFERS; // added by m.chase 03.06.2017
              comp->serialRecv_out(0,buff,serReadStat);
              // to avoid spinning, wait 50 ms
              Os::Task::delay(50);
              continue;
          }

//          timespec stime;
//          (void)clock_gettime(CLOCK_REALTIME,&stime);
//          DEBUG_PRINT("<<< Calling dsp_relay_uart_relay_read() at %d %d\n", stime.tv_sec, stime.tv_nsec);

          bool waiting = true;
          int stat = 0;

          while (waiting) {
              if (comp->m_quitReadThread) {
                  return;
              }

              stat = ::read(comp->m_fd,
                            buff.getData(),
                            buff.getSize());

              // Good read:
              if (stat > 0) {
                  sizeRead = stat;
                  // TODO remove
                  static int totalSizeRead = 0;
                  totalSizeRead += sizeRead;
                  //printf("<<< totalSizeRead: %d, readSize: %d\n", totalSizeRead, sizeRead);
                  //printf("<<< read size: %d\n", stat);
              }

              // check for timeout
              if (0 == stat) {
                  if (comp->m_quitReadThread) {
                      return;
                  }
              } else { // quit if other error or data
                  waiting = false;
              }
          }

          if (comp->m_quitReadThread) {
              return;
          }

          // check stat, maybe output event
          if (stat == -1) {
              // TODO(mereweth) - check errno
              Fw::LogStringArg _arg = comp->m_device;
              comp->log_WARNING_HI_DR_ReadError(_arg,stat);
              serReadStat = SerialReadStatus::SER_OTHER_ERR; // added by m.chase 03.06.2017
              //comp->serialRecv_out(0,buff,Drv::SER_OTHER_ERR);
          } else {
//              (void)clock_gettime(CLOCK_REALTIME,&stime);
//              DEBUG_PRINT("<!<! Sending data to RceAdapter %u at %d %d\n", buff.getsize(), stime.tv_sec, stime.tv_nsec);
              buff.setSize(sizeRead);
              serReadStat = SerialReadStatus::SER_OK; // added by m.chase 03.06.2017
              //comp->serialRecv_out(0,buff,Drv::SER_OK);
          }
          comp->serialRecv_out(0,buff,serReadStat); // added by m.chase 03.06.2017
      }
  }

  void LinuxSerialDriverComponentImpl ::
    startReadThread(NATIVE_UINT_TYPE priority, NATIVE_UINT_TYPE stackSize, NATIVE_UINT_TYPE cpuAffinity) {

      Os::TaskString task("SerReader");
      Os::Task::TaskStatus stat = this->m_readTask.start(task, serialReadTaskEntry, this, priority, stackSize, cpuAffinity);
      FW_ASSERT(stat == Os::Task::TASK_OK, stat);
  }

  void LinuxSerialDriverComponentImpl ::
    quitReadThread() {
      this->m_quitReadThread = true;
  }

} // end namespace Drv
