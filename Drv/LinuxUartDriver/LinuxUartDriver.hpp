// ======================================================================
// \title  LinuxUartDriverImpl.hpp
// \author tcanham
// \brief  hpp file for LinuxUartDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef LinuxUartDriver_HPP
#define LinuxUartDriver_HPP

#include <Drv/LinuxUartDriver/LinuxUartDriverComponentAc.hpp>
#include <Os/Mutex.hpp>
#include <Os/Task.hpp>

#include <termios.h>
#include <atomic>

namespace Drv {

class LinuxUartDriver final : public LinuxUartDriverComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object LinuxUartDriver
    //!
    LinuxUartDriver(const char* const compName /*!< The component name*/
    );

    //! Configure UART parameters
    enum UartBaudRate {
        BAUD_9600 = 9600,
        BAUD_19200 = 19200,
        BAUD_38400 = 38400,
        BAUD_57600 = 57600,
        BAUD_115K = 115200,
        BAUD_230K = 230400,
#ifdef TGT_OS_TYPE_LINUX
        BAUD_460K = 460800,
        BAUD_921K = 921600,
        BAUD_1000K = 1000000,
        BAUD_1152K = 1152000,
        BAUD_1500K = 1500000,
        BAUD_2000K = 2000000,
#ifdef B2500000
        BAUD_2500K = 2500000,
#endif
#ifdef B3000000
        BAUD_3000K = 3000000,
#endif
#ifdef B3500000
        BAUD_3500K = 3500000,
#endif
#ifdef B4000000
        BAUD_4000K = 4000000
#endif
#endif
    };

    enum UartFlowControl { NO_FLOW, HW_FLOW };

    enum UartParity { PARITY_NONE, PARITY_ODD, PARITY_EVEN };

    // Open device with specified baud and flow control.
    bool open(const char* const device,
              UartBaudRate baud,
              UartFlowControl fc,
              UartParity parity,
              FwSizeType allocationSize);

    //! start the serial poll thread.
    //! buffSize is the max receive buffer size
    //!
    void start(FwTaskPriorityType priority = Os::Task::TASK_PRIORITY_DEFAULT,
               Os::Task::ParamType stackSize = Os::Task::TASK_DEFAULT,
               Os::Task::ParamType cpuAffinity = Os::Task::TASK_DEFAULT);

    //! Quit thread
    void quitReadThread();

    //! Join thread
    Os::Task::Status join();

    //! Destroy object LinuxUartDriver
    //!
    ~LinuxUartDriver();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for run
    //!
    //! The rate group input for sending telemetry
    void run_handler(FwIndexType portNum,  //!< The port number
                     U32 context           //!< The call order
                     ) override;

    //! Handler implementation for serialSend
    //!
    Drv::ByteStreamStatus send_handler(FwIndexType portNum, /*!< The port number*/
                                       Fw::Buffer& serBuffer) override;

    //! Handler implementation for recvReturnIn
    //!
    //! Port receiving back ownership of data sent out on $recv port
    void recvReturnIn_handler(FwIndexType portNum,  //!< The port number
                              Fw::Buffer& fwBuffer  //!< The buffer
                              ) override;

    int m_fd;                     //!< file descriptor returned for I/O device
    FwSizeType m_allocationSize;  //!< size of allocation request to memory manager
    const char* m_device;         //!< original device path

    //! This method will be called by the new thread to wait for input on the serial port.
    static void serialReadTaskEntry(void* ptr);

    Os::Task m_readTask;  //!< task instance for thread to read serial port

    std::atomic<FwSizeType> m_bytesSent;      //!< number of bytes sent
    std::atomic<FwSizeType> m_bytesReceived;  //!< number of bytes received
    bool m_quitReadThread;                    //!< flag to quit thread
};

}  // end namespace Drv

#endif
