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
    // Functional tests
    // ----------------------------------------------------------------------

    void runNominalChannel();
    void runMultiChannel();
    void runOffNominal();

    //! Verify that Run_handler's CPU processing guard fires correctly when the
    //! number of updated telemetry entries exceeds TLMCHAN_MAX_ENTRIES_PER_RUN.
    //! Checks that deferred entries are counted, that a TlmChanEpochProcessingCapReached
    //! WARNING_HI event is emitted with the deferred count (and that no extra
    //! guard packet is injected into the downlink stream), and that
    //! m_procCapCount increments cumulatively across successive capped invocations.
    void runProcGuard();

    // ----------------------------------------------------------------------
    // doHash() cyber-security tests
    //
    // The original hash  doHash(id) = (id % MOD) % SLOTS  was a linear
    // transformation that let an attacker pre-compute channel IDs that all
    // map to the same slot, fill the free-bucket pool with junk, and shadow
    // legitimate telemetry channels (DoS + channel-shadowing attack).
    //
    // The replacement uses a seeded Murmur3 (≥32-bit IDs), Wang (16-bit), or
    // XOR-then-modulo (<16-bit) hash so that the slot mapping is
    // unpredictable without the per-boot seed.  The five tests below verify
    // each security property independently.
    // ----------------------------------------------------------------------

    //! Identify which hash width-branch is compiled in and verify that every
    //! output is within [0, TLMCHAN_NUM_TLM_HASH_SLOTS).  Prints a one-line
    //! note to stdout so the test log records the active code path.
    void runHashSizeIdentification();

    //! Confirm that repeated calls to doHash() with the same ID always return
    //! the same slot within a single session.  Determinism is required for
    //! correct table lookups; non-determinism here would be a regression.
    void runHashDeterminism();

    //! Confirm that two independently constructed TlmChan instances receive
    //! different random seeds, so an attacker cannot pre-compute a collision
    //! list for a target instance without capturing its seed at boot.
    //! Also verifies that the different seeds produce different slot mappings
    //! for the same set of channel IDs.
    void runHashSeedDiversity();

    //! Avalanche-effect test: flipping a single bit in a channel ID must
    //! change the hash output for a statistically significant fraction of
    //! cases.  A linear (or near-linear) hash fails this test, confirming the
    //! old vulnerability is gone.
    void runHashAvalanche();

    //! Direct regression against the original attack vector.
    //!
    //! Old exploit: channel IDs that are multiples of TLMCHAN_NUM_TLM_HASH_SLOTS
    //! all mapped to slot 0 under the linear hash, letting an attacker fill the
    //! free-bucket pool without any secret knowledge.
    //!
    //! This test generates exactly those IDs and verifies that the seeded hash
    //! scatters them across slots so that no single slot accumulates enough
    //! collisions to exhaust TLMCHAN_HASH_BUCKETS.
    void runCollisionHardening();

    //! Statistical uniformity test: hash a large population of sequential IDs
    //! and confirm that the slot occupancy counts fall within a reasonable
    //! band around the expected value.  Hot-spot slots weaken collision
    //! resistance and degrade lookup performance.
    void runHashDistribution();

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
