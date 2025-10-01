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
