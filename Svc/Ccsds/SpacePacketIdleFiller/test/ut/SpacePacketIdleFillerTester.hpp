// ======================================================================
// \title  SpacePacketIdleFillerTester.hpp
// \author claradavisb
// \brief  hpp file for SpacePacketIdleFiller component test harness implementation class
// ======================================================================

#ifndef Svc_Ccsds_SpacePacketIdleFillerTester_HPP
#define Svc_Ccsds_SpacePacketIdleFillerTester_HPP

#include "Svc/Ccsds/SpacePacketIdleFiller/SpacePacketIdleFiller.hpp"
#include "Svc/Ccsds/SpacePacketIdleFiller/SpacePacketIdleFillerGTestBase.hpp"

namespace Svc {

namespace Ccsds {

class SpacePacketIdleFillerTester final : public SpacePacketIdleFillerGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    static const FwSizeType MAX_HISTORY_SIZE = 20;
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    //! Size every emitted buffer is padded to. Small enough to check byte by byte.
    static const FwSizeType TEST_TARGET_SIZE = 64;

    //! A well-formed space packet is a 6-byte header plus at least one byte of data
    static const FwSizeType MIN_IDLE_PACKET_SIZE = 7;

    //! Backing storage for buffers handed to the component
    static const FwSizeType TEST_BUFFER_SIZE = 128;

  public:
    SpacePacketIdleFillerTester();
    ~SpacePacketIdleFillerTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! A short buffer is emitted at the target size with the original bytes intact.
    void testPadsToTarget();

    //! The padding is a spec-conformant idle space packet.
    void testIdlePacketFormat();

    //! A gap of exactly the minimum yields an idle packet with a single data byte
    void testMinimumGap();

    //! A buffer already at the target is emitted unpadded.
    void testExactFitNoPadding();

    //! A buffer above the target is dropped with an event, and the com status token released.
    void testInputTooLarge();

    //! A gap too small to hold a space packet is dropped with an event, and the com status token released
    void testGapTooSmall();

    //! The frame context travels with the padded buffer unchanged
    void testContextForwarded();

    //! The incoming buffer is handed back as soon as it has been copied.
    void testInputReturned();

    //! Returning the emitted buffer frees the storage for the next one.
    void testStorageReusedAfterReturn();

    //! A downstream status is passed upstream unchanged
    void testStatusForwarded();

  private:
    // ----------------------------------------------------------------------
    // Helpers
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

    //! Hand the component a buffer of the given size, filled with a recognizable ramp
    Fw::Buffer sendData(FwSizeType size);

    //! Return the most recently emitted buffer to the component
    void returnEmitted();

    //! The emitted buffer from the given dataOut history entry
    Fw::Buffer emitted(U32 index);

    //! The component under test
    SpacePacketIdleFiller component;

    //! Backing storage for buffers handed to the component
    U8 m_bufferStorage[TEST_BUFFER_SIZE];
};

}  // namespace Ccsds

}  // namespace Svc

#endif
