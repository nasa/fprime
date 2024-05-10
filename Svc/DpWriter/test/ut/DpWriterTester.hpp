// ======================================================================
// \title  DpWriterTester.hpp
// \author bocchino
// \brief  hpp file for DpWriter component test harness implementation class
// ======================================================================

#ifndef Svc_DpWriterTester_HPP
#define Svc_DpWriterTester_HPP

#include "Svc/DpWriter/DpWriter.hpp"
#include "Svc/DpWriter/DpWriterGTestBase.hpp"
#include "Svc/DpWriter/test/ut/AbstractState.hpp"

namespace Svc {

class DpWriterTester : public DpWriterGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const NATIVE_INT_TYPE MAX_HISTORY_SIZE = 10;

    // Instance ID supplied to the component instance under test
    static const NATIVE_INT_TYPE TEST_INSTANCE_ID = 0;

    // Queue depth supplied to the component instance under test
    static const NATIVE_INT_TYPE TEST_INSTANCE_QUEUE_DEPTH = 10;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object DpWriterTester
    DpWriterTester();

    //! Destroy object DpWriterTester
    ~DpWriterTester();

  private:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Handler implementation for procBufferSendOut
    void from_procBufferSendOut_handler(NATIVE_INT_TYPE portNum,  //!< The port number
                                        Fw::Buffer& fwBuffer      //!< The buffer
    ) final;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Print events
    void printEvents();

  protected:
    // ----------------------------------------------------------------------
    // Protected helper functions
    // ----------------------------------------------------------------------

    //! Pick an Os status other than OP_OK
    //! \return The status
    static Os::File::Status pickOsFileError();

    //! Construct a DP file name
    void constructDpFileName(FwDpIdType id,            //!< The container ID (input)
                             const Fw::Time& timeTag,  //!< The time tag (input)
                             Fw::StringBase& fileName  //!< The file name (output)
    );

    //! Check processing types
    void checkProcTypes(const Fw::DpContainer& container  //!< The container
    );

    //! Check telemetry
    void checkTelemetry();

  private:
    // ----------------------------------------------------------------------
    // Private helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

  protected:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The abstract state for testing
    AbstractState abstractState;

    //! The component under test
    DpWriter component;
};

}  // namespace Svc

#endif
