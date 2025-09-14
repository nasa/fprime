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
    static const U32 MAX_HISTORY_SIZE = 10;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    // Queue depth supplied to the component instance under test
    static const FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 10;

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
    void from_procBufferSendOut_handler(FwIndexType portNum,  //!< The port number
                                        Fw::Buffer& fwBuffer  //!< The buffer
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

  public:
    // ----------------------------------------------------------------------
    // Accessor methods for protected/private members
    // ----------------------------------------------------------------------

    //! Dispatch a message
    void doDispatch();

    //! Get the EVENTID_INVALIDBUFFER_THROTTLE value
    static FwSizeType getInvalidBufferThrottle() { return DpWriterComponentBase::EVENTID_INVALIDBUFFER_THROTTLE; }

    //! Get the EVENTID_BUFFERTOOSMALLFORPACKET_THROTTLE value
    static FwSizeType getBufferTooSmallForPacketThrottle() {
        return DpWriterComponentBase::EVENTID_BUFFERTOOSMALLFORPACKET_THROTTLE;
    }

    //! Get the EVENTID_INVALIDHEADERHASH_THROTTLE value
    static FwSizeType getInvalidHeaderHashThrottle() {
        return DpWriterComponentBase::EVENTID_INVALIDHEADERHASH_THROTTLE;
    }

    //! Get the EVENTID_INVALIDHEADER_THROTTLE value
    static FwSizeType getInvalidHeaderThrottle() { return DpWriterComponentBase::EVENTID_INVALIDHEADER_THROTTLE; }

    //! Get the EVENTID_FILEOPENERROR_THROTTLE value
    static FwSizeType getFileOpenErrorThrottle() { return DpWriterComponentBase::EVENTID_FILEOPENERROR_THROTTLE; }

    //! Get the EVENTID_FILEWRITEERROR_THROTTLE value
    static FwSizeType getFileWriteErrorThrottle() { return DpWriterComponentBase::EVENTID_FILEWRITEERROR_THROTTLE; }

    //! Get the OPCODE_CLEAR_EVENT_THROTTLE value
    static FwOpcodeType getOpCodeClearEventThrottle() { return DpWriterComponentBase::OPCODE_CLEAR_EVENT_THROTTLE; }

    //! Get the m_BufferTooSmallForDataThrottle value
    FwIndexType getBufferTooSmallForDataThrottleCount();

    //! Get the m_BufferTooSmallForPacketThrottle value
    FwIndexType getBufferTooSmallForPacketThrottleCount();

    //! Get the m_FileOpenErrorThrottle value
    FwIndexType getFileOpenErrorThrottleCount();

    //! Get the m_FileWriteErrorThrottle value
    FwIndexType getFileWriteErrorThrottleCount();

    //! Get the m_InvalidBufferThrottle value
    FwIndexType getInvalidBufferThrottleCount();

    //! Get the m_InvalidHeaderHashThrottle value
    FwIndexType getInvalidHeaderHashThrottleCount();

    //! Get the m_InvalidHeaderThrottle value
    FwIndexType getInvalidHeaderThrottleCount();
};

}  // namespace Svc

#endif
