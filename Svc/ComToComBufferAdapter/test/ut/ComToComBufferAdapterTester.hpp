// ======================================================================
// \title  ComToComBufferAdapterTester.hpp
// \brief  hpp file for ComToComBufferAdapter component test harness
// ======================================================================

#ifndef Svc_ComToComBufferAdapterTester_HPP
#define Svc_ComToComBufferAdapterTester_HPP

#include "Svc/ComToComBufferAdapter/ComToComBufferAdapter.hpp"
#include "Svc/ComToComBufferAdapter/ComToComBufferAdapterGTestBase.hpp"

namespace Svc {

class ComToComBufferAdapterTester : public ComToComBufferAdapterGTestBase {
  public:
    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;
    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object ComToComBufferAdapterTester
    ComToComBufferAdapterTester();

    //! Destroy object ComToComBufferAdapterTester
    ~ComToComBufferAdapterTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Nominal: descriptor is stripped and forwarded as the APID
    void testNominal();

    //! A zero-length payload (descriptor only) is forwarded as an empty buffer
    void testEmptyPayload();

  private:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Handler for from_comBufferSendOut
    void from_comBufferSendOut_handler(FwIndexType portNum,             //!< The port number
                                       Fw::ComBuffer& data,             //!< Buffer containing packet data
                                       const ComCfg::Apid& packetType,  //!< Packet APID
                                       U32 context                      //!< Call context value
                                       ) override;

  private:
    // ----------------------------------------------------------------------
    // Helper methods
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
    ComToComBufferAdapter component;
};

}  // namespace Svc

#endif
