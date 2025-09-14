// ======================================================================
// \title  ComStub.hpp
// \author mstarch
// \brief  hpp file for ComStub component implementation class
// ======================================================================

#ifndef Svc_ComStub_HPP
#define Svc_ComStub_HPP

#include "Drv/ByteStreamDriverModel/ByteStreamStatusEnumAc.hpp"
#include "Svc/ComStub/ComStubComponentAc.hpp"

namespace Svc {

class ComStub final : public ComStubComponentBase {
    friend class ComStubTester;  //!< Allow UT Tester to access private members

  public:
    const FwIndexType RETRY_LIMIT = 10;
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object ComStub
    //!
    ComStub(const char* const compName /*!< The component name*/
    );

    //! Destroy object ComStub
    //!
    ~ComStub() override;

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------
  private:
    //! Handler implementation for dataIn
    //!
    //! Comms data is coming in meaning there is a request for ComStub to send data on the wire
    //! For ComStub, this means we send the data to the underlying driver (e.g. TCP/UDP/UART)
    void dataIn_handler(const FwIndexType portNum, /*!< The port number*/
                        Fw::Buffer& sendBuffer,
                        const ComCfg::FrameContext& context) override;

    //! Handler implementation for drvConnected
    //!
    void drvConnected_handler(const FwIndexType portNum) override;

    //! Handler implementation for drvReceiveIn
    //!
    //! Data is coming in from the driver (meaning it has been read from the wire).
    //! ComStub forwards this to the dataOut port
    void drvReceiveIn_handler(const FwIndexType portNum,
                              /*!< The port number*/ Fw::Buffer& recvBuffer,
                              const Drv::ByteStreamStatus& recvStatus) override;

    //! Handler implementation for dataReturnIn
    //!
    //! Port receiving back ownership of buffer sent out on dataOut
    void dataReturnIn_handler(FwIndexType portNum,   //!< The port number
                              Fw::Buffer& fwBuffer,  //!< The buffer
                              const ComCfg::FrameContext& context) override;

    //! Handler implementation for drvAsyncSendReturnIn
    //!
    //! Buffer ownership and status returning from an async driver "send" operation (async callback)
    void drvAsyncSendReturnIn_handler(FwIndexType portNum,   //!< The port number
                                      Fw::Buffer& fwBuffer,  //!< The buffer
                                      const Drv::ByteStreamStatus& recvStatus) override;

    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------
  private:
    //! Handle synchronous sending of data
    void handleSynchronousSend(Fw::Buffer& sendBuffer, const ComCfg::FrameContext& context);

    //! Handle asynchronous sending of data
    void handleAsynchronousSend(Fw::Buffer& sendBuffer, const ComCfg::FrameContext& context);

    //! Handle retry logic for asynchronous sends
    void handleAsyncRetry(Fw::Buffer& fwBuffer);

    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------
  private:
    bool m_reinitialize;                   //!< Stores if a ready signal is needed on connection
    ComCfg::FrameContext m_storedContext;  //!< Keep context of the last message sent in the asynchronous case
    FwIndexType m_retry_count;             //!< Keep track of retry count in the asynchronous case
};

}  // end namespace Svc

#endif
