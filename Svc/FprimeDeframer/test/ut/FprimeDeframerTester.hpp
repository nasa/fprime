// ======================================================================
// \title  FprimeDeframerTester.hpp
// \author thomas-bc
// \brief  hpp file for FprimeDeframer component test harness implementation class
// ======================================================================

#ifndef Svc_FprimeDeframerTester_HPP
#define Svc_FprimeDeframerTester_HPP

#include "Svc/FprimeDeframer/FprimeDeframer.hpp"
#include "Svc/FprimeDeframer/FprimeDeframerGTestBase.hpp"

namespace Svc {

class FprimeDeframerTester : public FprimeDeframerGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object FprimeDeframerTester
    FprimeDeframerTester();

    //! Destroy object FprimeDeframerTester
    ~FprimeDeframerTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Test receiving a nominal frame
    void testNominalFrame();

    //! Test receiving a nominal frame that contains a valid packet (with APID)
    void testNominalFrameApid();

    //! Test receiving a truncated frame
    void testTruncatedFrame();

    //! Test receiving a zero size frame
    void testZeroSizeFrame();

    //! Test receiving a frame with an incorrect length token (too long for the data)
    void testIncorrectLengthToken();

    //! Test receiving a frame with an incorrect start word
    void testIncorrectStartWord();

    //! Test receiving a frame with an incorrect Crc field
    void testIncorrectCrc();

    //! Test bufferReturn passthrough
    void testDataReturn();

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

    //! Takes in a buffer of data and size, and injects a checksum into the buffer
    //! If the buffer doesn't have enough room for the checksum (4 bytes), the buffer is left unchanged
    void injectChecksum(U8* data, FwSizeType size);

    //! Sends a buffer of supplied data and size on the component input port
    void mockReceiveData(U8* data, FwSizeType size);

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    FprimeDeframer component;
};

}  // namespace Svc

#endif
