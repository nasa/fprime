// ======================================================================
// \title  LinuxUartDriverTester.hpp
// \author tcanham
// \brief  hpp file for LinuxUartDriver component test harness implementation class
// ======================================================================

#ifndef Drv_LinuxUartDriverTester_HPP
#define Drv_LinuxUartDriverTester_HPP

#include "Drv/LinuxUartDriver/LinuxUartDriver.hpp"
#include "Drv/LinuxUartDriver/LinuxUartDriverGTestBase.hpp"

namespace Drv {

class LinuxUartDriverTester : public LinuxUartDriverGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const NATIVE_INT_TYPE MAX_HISTORY_SIZE = 10;

    // Instance ID supplied to the component instance under test
    static const NATIVE_INT_TYPE TEST_INSTANCE_ID = 0;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object LinuxUartDriverTester
    LinuxUartDriverTester();

    //! Destroy object LinuxUartDriverTester
    ~LinuxUartDriverTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! To do
    void toDo();

  private:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Handler implementation for allocate
    Fw::Buffer from_allocate_handler(NATIVE_INT_TYPE portNum,  //!< The port number
                                     U32 size);

    //! Handler implementation for deallocate
    void from_deallocate_handler(NATIVE_INT_TYPE portNum,  //!< The port number
                                 Fw::Buffer& fwBuffer);

    //! Handler implementation for ready
    void from_ready_handler(NATIVE_INT_TYPE portNum  //!< The port number
    );

    //! Handler implementation for recv
    void from_recv_handler(NATIVE_INT_TYPE portNum,  //!< The port number
                           Fw::Buffer& recvBuffer,
                           const Drv::RecvStatus& recvStatus);

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    LinuxUartDriver component;
};

}  // namespace Drv

#endif
