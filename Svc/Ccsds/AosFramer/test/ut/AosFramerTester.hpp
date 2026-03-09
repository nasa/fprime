// ======================================================================
// \title  AosFramerTester.hpp
// \author Will MacCormack (Aos Modifications)
// \brief  hpp file for AosFramer component test harness implementation class
// \details modified from thomas-bc's TmFramer
// ======================================================================

#ifndef Svc_Ccsds_AosFramerTester_HPP
#define Svc_Ccsds_AosFramerTester_HPP

#include "Svc/Ccsds/AosFramer/AosFramer.hpp"
#include "Svc/Ccsds/AosFramer/AosFramerGTestBase.hpp"

namespace Svc {

namespace Ccsds {

class AosFramerTester final : public AosFramerGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 20;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object AosFramerTester
    AosFramerTester();

    //! Destroy object AosFramerTester
    ~AosFramerTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void testComStatusPassthrough();
    void testNominalFraming();
    void testSeqCountWrapAround();
    void testDataReturn();
    void testBufferOwnershipState();
    void testLongPacket();
    void testShortPackets();

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

    U8 getFrameTfVn(U8* frameData);      //!< Get the transfer frame version number from the frame - no boundary check
    U16 getFrameScId(U8* frameData);     //!< Get the Spacecraft ID from the frame - no boundary check
    U8 getFrameVcId(U8* frameData);      //!< Get the Virtual Channel ID from the frame - no boundary check
    U32 getFrameVcCount(U8* frameData);  //!< Get the Virtual Frame Count from the frame - no boundary check
    U16 getFramePacketPointer(U8* frameData);  //!< Get the M_PDU First Header Pointer - no boundary check

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    AosFramer component;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
