// ======================================================================
// \title  DpTool/test/ut/Tester.hpp
// \author bocchino
// \brief  hpp file for DpTool test harness implementation class
// ======================================================================

#ifndef FppTest_DpTool_Tester_HPP
#define FppTest_DpTool_Tester_HPP

#include "DpToolGTestBase.hpp"
#include "FppTest/dpTool/DpTool.hpp"
#include "Fw/Dp/test/util/DpContainerHeader.hpp"
#include "STest/Pick/Pick.hpp"

namespace FppTest {

class Tester : public DpToolGTestBase {
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

  public:
    // Maximum size of histories storing events, telemetry, and port outputs
    static constexpr FwSizeType MAX_HISTORY_SIZE = 10;
    // Instance ID supplied to the component instance under test
    static constexpr FwSizeType TEST_INSTANCE_ID = 0;
    // Queue depth supplied to component instance under test
    static constexpr FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 10;
    // The component id base
    static constexpr FwDpIdType ID_BASE = 100;

    //! Construct object Tester
    //!
    Tester();

    //! Destroy object Tester
    //!
    ~Tester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! schedIn OK
    void schedIn_OK();

    // Test Tests
    void makeU32();
    void makeComplex();
    void makeU8Array();
    void makeU32Array();
    void makeDataArray();
    void makeEnum();
    void makeF32();
    void makeF64();
    void makeBool();
    void makeI8();
    void makeI16();
    void makeI32();
    void makeI64();
    void makeFppArray();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handlers for data product ports
    // ----------------------------------------------------------------------

    Fw::Success::T productGet_handler(FwDpIdType id,      //!< The container ID
                                      FwSizeType size,    //!< The size of the requested buffer
                                      Fw::Buffer& buffer  //!< The buffer
                                      ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    void writeBinaryFile(const char* fileName, Fw::Buffer buffer);

    //! Connect ports
    //!
    void connectPorts();

    //! Initialize components
    //!
    void initComponents();

    //! Set and return a random time
    //! \return The time
    Fw::Time randomizeTestTime();

    //! Invoke productRecvIn and check header
    //! This sets the output buffer to the received buffer and sets the
    //! deserialization pointer to the start of the data payload
    void productRecvIn_InvokeAndCheckHeader(FwDpIdType id,               //!< The container id
                                            FwSizeType dataEltSize,      //!< The data element size
                                            FwDpPriorityType priority,   //!< The priority
                                            Fw::Buffer inputBuffer,      //!< The buffer to send
                                            Fw::Buffer& outputBuffer,    //!< The buffer received (output)
                                            FwSizeType& expectedNumElts  //!< The expected number of elements (output)
    );

    //! Check received buffer with failure status
    void productRecvIn_CheckFailure(FwDpIdType id,     //!< The container id
                                    Fw::Buffer buffer  //!< The buffer
    );

  PRIVATE:
    // ----------------------------------------------------------------------
    // Variables
    // ----------------------------------------------------------------------

   //! Buffer data for Container 1
    U8 container1Data[DpTool::CONTAINER_1_PACKET_SIZE];
    //! Buffer for Container 1
    const Fw::Buffer container1Buffer;

    //! The component under test
    DpTool component;
};

}  // end namespace FppTest

#endif
