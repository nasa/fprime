// ======================================================================
// \title  DpCompressProcTester.hpp
// \author kubiak
// \brief  hpp file for DpCompressProc component test harness implementation class
// ======================================================================

#ifndef Svc_DpCompressProcTester_HPP
#define Svc_DpCompressProcTester_HPP

#include "Svc/DpCompressProc/DpCompressProc.hpp"
#include "Svc/DpCompressProc/DpCompressProcGTestBase.hpp"
#include "Svc/DpCompressProc/test/ut/AbstractState.hpp"

namespace Svc {

class DpCompressProcTester : public DpCompressProcGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 1000;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object DpCompressProcTester
    DpCompressProcTester();

    //! Destroy object DpCompressProcTester
    ~DpCompressProcTester();

    void uncompress_data(Fw::Buffer container_buf, const FwSizeStoreType chunk_size, std::vector<U8>& out_vec);

    void test_chunks(const FwSizeStoreType chunk_size, std::vector<AbstractState::Chunk> chunks);

    void test_chunks_disabled(const FwSizeStoreType chunk_size, std::vector<AbstractState::Chunk> chunks);

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

  private:
    void test_chunks_helper(const FwSizeStoreType chunk_size,
                            std::vector<AbstractState::Chunk> chunks,
                            Fw::Buffer container_buf);

    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------
    Svc::CompressionAlgorithm from_compressChunk_handler(FwIndexType portNum,  //!< The port number
                                                         Fw::Buffer& buffer,
                                                         FwSizeType min_compression,
                                                         FwSizeType write_offset);

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

  public:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The abstract state for testing
    AbstractState abstractState;

    //! The component under test
    DpCompressProc component;
};

}  // namespace Svc

#endif
