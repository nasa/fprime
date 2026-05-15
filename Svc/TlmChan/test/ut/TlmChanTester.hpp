// ======================================================================
// \title  TlmChan/test/ut/Tester.hpp
// \author tcanham
// \brief  hpp file for TlmChan test harness implementation class
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "Svc/TlmChan/TlmChan.hpp"
#include "TlmChanGTestBase.hpp"

namespace Svc {

class TlmChanTester : public TlmChanGTestBase {
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

  public:
    //! Construct object TlmChanTester
    //!
    TlmChanTester();

    //! Destroy object TlmChanTester
    //!
    ~TlmChanTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void runNominalChannel();
    void runMultiChannel();
    void runOffNominal();

    //! Verify that Run_handler's CPU processing guard fires correctly when the
    //! number of updated telemetry entries exceeds TLMCHAN_MAX_ENTRIES_PER_RUN.
    //! Checks that deferred entries are counted, a guard packet is appended to
    //! the downlink stream, and m_procCapCount increments cumulatively across
    //! successive capped invocations.
    void runProcGuard();

  private:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Handler for from_PktSend
    //!
    void from_PktSend_handler(const FwIndexType portNum,  //!< The port number
                              Fw::ComBuffer& data,        //!< Buffer containing packet data
                              U32 context                 //!< Call context value; meaning chosen by user
    );

    //! Handler for from_pingOut
    //!
    void from_pingOut_handler(const FwIndexType portNum,  //!< The port number
                              U32 key                     //!< Value to return to pinger
    );

  private:
    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    //! Connect ports
    //!
    void connectPorts();

    //! Initialize components
    //!
    void initComponents();

    void sendBuff(FwChanIdType id, U32 val);
    bool doRun(bool check);
    void checkBuff(FwChanIdType chanNum, FwChanIdType totalChan, FwChanIdType id, U32 val);

    void clearBuffs();

    //! Verify the ProcCapReached event emitted by Run_handler when the
    //! processing cap was reached.  Asserts that exactly one event was
    //! emitted for the current run and that its parameters match.
    //!
    void checkGuardEvent(U32 expectedDeferred, U32 expectedCapCount);

    // dump functions
    void dumpHash();
    static void dumpTlmEntry(TlmChan::TlmEntry* entry);

  private:
    // ----------------------------------------------------------------------
    // Variables
    // ----------------------------------------------------------------------

    //! The component under test
    //!
    TlmChan component;
    // Keep a history
    FwChanIdType m_numBuffs;
    Fw::ComBuffer m_rcvdBuffer[TLMCHAN_HASH_BUCKETS];
    bool m_bufferRecv;
};

}  // end namespace Svc

#endif
