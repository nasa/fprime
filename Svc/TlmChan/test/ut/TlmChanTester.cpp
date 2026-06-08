// ======================================================================
// \title  TlmChan.hpp
// \author tcanham
// \brief  cpp file for TlmChan test harness implementation class
// ======================================================================

#include "TlmChanTester.hpp"
#include <Fw/Test/UnitTest.hpp>
#include <config/TlmChanImplCfg.hpp>

#define INSTANCE 0
#define QUEUE_DEPTH 10

// MAX_HISTORY_SIZE must cover the worst-case number of PktSend port calls in
// a single doRun invocation.  In ProcGuardTest, Run_handler sends up to
// ceil(TLMCHAN_MAX_ENTRIES_PER_RUN / CHANS_PER_COMBUFFER) app packets in one
// call.  TLMCHAN_HASH_BUCKETS is a safe upper bound since
// TLMCHAN_MAX_ENTRIES_PER_RUN < TLMCHAN_HASH_BUCKETS.
#define MAX_HISTORY_SIZE (TLMCHAN_HASH_BUCKETS + 1)

static const FwChanIdType TEST_CHAN_SIZE = sizeof(FwChanIdType) + Fw::Time::SERIALIZED_SIZE + sizeof(U32);
static const FwChanIdType CHANS_PER_COMBUFFER =
    (FW_COM_BUFFER_MAX_SIZE - sizeof(FwPacketDescriptorType)) / TEST_CHAN_SIZE;
static constexpr FwSizeType INTEGER_DIVISION_ROUNDED_UP(FwSizeType a, FwSizeType b) {
    return ((a % b) == 0) ? (a / b) : (a / b) + 1;
}

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

TlmChanTester ::TlmChanTester()
    : TlmChanGTestBase("Tester", MAX_HISTORY_SIZE), component("TlmChan"), m_numBuffs(0), m_bufferRecv(false) {
    this->initComponents();
    this->connectPorts();
}

TlmChanTester ::~TlmChanTester() {
    this->component.deinit();
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void TlmChanTester::runNominalChannel() {
    this->clearBuffs();
    // send first buffer
    this->sendBuff(27, 10);
    this->doRun(true);
    this->checkBuff(0, 1, 27, 10);

    this->clearBuffs();
    // send again to other buffer
    this->sendBuff(27, 10);

    static bool tlc003 = false;

    if (not tlc003) {
        REQUIREMENT("TLC-003");
        tlc003 = true;
    }

    this->doRun(true);
    this->checkBuff(0, 1, 27, 10);

    // do an update to make sure it gets updated and returned correctly
    this->clearBuffs();
    this->sendBuff(27, 20);
}

void TlmChanTester::runMultiChannel() {
    FwChanIdType ID_0[] = {// Test channel IDs
                           0x1000, 0x1001, 0x1002, 0x1003, 0x1004, 0x1005, 0x1100, 0x1101, 0x1102, 0x1103, 0x300,
                           0x301,  0x400,  0x401,  0x402,  0x100,  0x101,  0x102,  0x103,  0x104,  0x105};

    this->clearBuffs();
    // send all updates
    for (FwChanIdType n = 0; n < FW_NUM_ARRAY_ELEMENTS(ID_0); n++) {
        this->sendBuff(ID_0[n], static_cast<U32>(n));
    }

    ASSERT_EQ(TlmChan::ActiveBuffer::Buffer_0, this->component.m_activeBuffer);

    // do a run, and all the packets should be sent
    this->doRun(true);
    ASSERT_TRUE(this->m_bufferRecv);
    ASSERT_EQ(INTEGER_DIVISION_ROUNDED_UP(FW_NUM_ARRAY_ELEMENTS(ID_0), CHANS_PER_COMBUFFER), this->m_numBuffs);
    ASSERT_EQ(TlmChan::ActiveBuffer::Buffer_1, this->component.m_activeBuffer);

    // verify packets
    for (FwChanIdType n = 0; n < FW_NUM_ARRAY_ELEMENTS(ID_0); n++) {
        this->checkBuff(n, FW_NUM_ARRAY_ELEMENTS(ID_0), ID_0[n], static_cast<U32>(n));
    }

    // send another set

    FwChanIdType ID_1[] = {// Test channel IDs
                           0x5000, 0x5001, 0x5002, 0x5003, 0x5004, 0x5005, 0x5100, 0x5101, 0x5102,
                           0x5103, 0x6300, 0x6301, 0x6400, 0x6401, 0x6402, 0x6100, 0x6101, 0x6102,
                           0x6103, 0x6104, 0x6105, 0x8101, 0x8102, 0x8103, 0x8104, 0x8105};

    this->clearBuffs();
    // send all updates
    for (FwChanIdType n = 0; n < FW_NUM_ARRAY_ELEMENTS(ID_1); n++) {
        this->sendBuff(ID_1[n], static_cast<U32>(n));
    }

    ASSERT_EQ(TlmChan::ActiveBuffer::Buffer_1, this->component.m_activeBuffer);

    // do a run, and all the packets should be sent
    this->doRun(true);
    ASSERT_TRUE(this->m_bufferRecv);
    ASSERT_EQ(INTEGER_DIVISION_ROUNDED_UP(FW_NUM_ARRAY_ELEMENTS(ID_1), CHANS_PER_COMBUFFER), this->m_numBuffs);
    ASSERT_EQ(TlmChan::ActiveBuffer::Buffer_0, this->component.m_activeBuffer);

    // verify packets
    for (FwChanIdType n = 0; n < FW_NUM_ARRAY_ELEMENTS(ID_1); n++) {
        this->checkBuff(n, FW_NUM_ARRAY_ELEMENTS(ID_1), ID_1[n], static_cast<U32>(n));
    }
}

void TlmChanTester::runOffNominal() {
    // Ask for a packet that isn't written yet
    Fw::TlmBuffer buff;
    Fw::SerializeStatus stat;
    Fw::Time timeTag;
    U32 val = 10;

    // create Telemetry item and put dummy data in to make sure it gets erased
    buff.resetSer();
    stat = buff.serializeFrom(val);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat);

    // Read back value
    Fw::TlmValid valid = this->invoke_to_TlmGet(0, 10, timeTag, buff);
    ASSERT_EQ(0u, buff.getSize());
    ASSERT_EQ(valid, Fw::TlmValid::INVALID);
}

void TlmChanTester::runProcGuard() {
    // This test only has meaning when the per-run cap is set below the total
    // bucket count.  If TLMCHAN_MAX_ENTRIES_PER_RUN equals TLMCHAN_HASH_BUCKETS
    // the guard can never fire.
    if (TLMCHAN_MAX_ENTRIES_PER_RUN >= TLMCHAN_HASH_BUCKETS) {
        printf(
            "SKIP: TLMCHAN_MAX_ENTRIES_PER_RUN (%u) >= TLMCHAN_HASH_BUCKETS (%u); "
            "guard cannot fire with current config\n",
            static_cast<unsigned>(TLMCHAN_MAX_ENTRIES_PER_RUN), static_cast<unsigned>(TLMCHAN_HASH_BUCKETS));
        return;
    }

    // Fill every bucket to guarantee the cap is exceeded regardless of its
    // exact value.  Channel IDs start at 0x3000 to avoid collisions with
    // other tests.
    const U32 numToSend = TLMCHAN_HASH_BUCKETS;
    const U32 expectedDeferred = TLMCHAN_HASH_BUCKETS - TLMCHAN_MAX_ENTRIES_PER_RUN;

    printf("ProcGuard config: TLMCHAN_HASH_BUCKETS=%u  TLMCHAN_MAX_ENTRIES_PER_RUN=%u  expectedDeferred=%u\n",
           static_cast<unsigned>(TLMCHAN_HASH_BUCKETS), static_cast<unsigned>(TLMCHAN_MAX_ENTRIES_PER_RUN),
           static_cast<unsigned>(expectedDeferred));

    // Expected app packets: only the processed (non-deferred) entries are serialized.
    // With events replacing the guard packet, m_numBuffs must equal exactly this
    // value — no extra guard packet is injected into the downlink stream.
    const FwChanIdType appPackets =
        static_cast<FwChanIdType>(INTEGER_DIVISION_ROUNDED_UP(TLMCHAN_MAX_ENTRIES_PER_RUN, CHANS_PER_COMBUFFER));

    // -----------------------------------------------------------------------
    // First run: fill all buckets — guard must fire for the first time
    // -----------------------------------------------------------------------
    this->clearBuffs();

    for (U32 n = 0; n < numToSend; n++) {
        this->sendBuff(static_cast<FwChanIdType>(0x3000u + n), n);
    }

    this->clearHistory();
    this->doRun(true);

    // Internal cap counter must have incremented
    ASSERT_EQ(1u, this->component.m_procCapCount);

    // Only app packets in downlink — no extra guard packet
    ASSERT_EQ(appPackets, this->m_numBuffs);

    // Exactly one ProcCapReached event with correct parameters
    this->checkGuardEvent(expectedDeferred, 1u);

    // -----------------------------------------------------------------------
    // Second run: no channels updated (buffer swap cleared them all).
    // Guard must NOT fire and m_procCapCount must remain at 1.
    // -----------------------------------------------------------------------
    this->clearBuffs();
    this->clearHistory();
    this->doRun(false);

    ASSERT_EQ(1u, this->component.m_procCapCount);
    ASSERT_EVENTS_SIZE(0);

    // -----------------------------------------------------------------------
    // Third run: update same channels again to verify cumulative count.
    // TlmRecv finds existing buckets by ID so no new allocation occurs.
    // -----------------------------------------------------------------------
    this->clearBuffs();

    for (U32 n = 0; n < numToSend; n++) {
        this->sendBuff(static_cast<FwChanIdType>(0x3000u + n), n + 100u);
    }

    this->clearHistory();
    this->doRun(true);

    // m_procCapCount must be 2 — cumulative across both capped invocations
    ASSERT_EQ(2u, this->component.m_procCapCount);

    ASSERT_EQ(appPackets, this->m_numBuffs);

    // deferred count same each cycle; cumulative count is now 2
    this->checkGuardEvent(expectedDeferred, 2u);
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void TlmChanTester ::from_PktSend_handler(const FwIndexType portNum, Fw::ComBuffer& data, U32 context) {
    this->pushFromPortEntry_PktSend(data, context);
    this->m_bufferRecv = true;
    this->m_rcvdBuffer[this->m_numBuffs] = data;
    this->m_numBuffs++;
}

void TlmChanTester ::from_pingOut_handler(const FwIndexType portNum, U32 key) {
    this->pushFromPortEntry_pingOut(key);
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

bool TlmChanTester::doRun(bool check) {
    // execute run port to send packet
    this->invoke_to_Run(0, 0);
    // dispatch run message
    this->m_bufferRecv = false;
    this->component.doDispatch();
    if (check) {
        EXPECT_TRUE(this->m_bufferRecv);
    }
    return this->m_bufferRecv;
}

void TlmChanTester::checkBuff(FwChanIdType chanNum, FwChanIdType totalChan, FwChanIdType id, U32 val) {
    Fw::Time timeTag;
    Fw::SerializeStatus stat;

    static bool tlc004 = false;

    if (not tlc004) {
        REQUIREMENT("TLC-004");
        tlc004 = true;
    }

    FwChanIdType currentChan = 0;

    for (FwChanIdType packet = 0; packet < this->m_numBuffs; packet++) {
        this->m_rcvdBuffer[packet].resetDeser();
        FwPacketDescriptorType desc;
        stat = this->m_rcvdBuffer[packet].deserializeTo(desc);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat);
        ASSERT_EQ(desc, static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_TELEM));

        for (FwChanIdType chan = 0; chan < CHANS_PER_COMBUFFER; chan++) {
            FwEventIdType sentId;
            stat = this->m_rcvdBuffer[packet].deserializeTo(sentId);
            ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat);

            Fw::Time recTimeTag(TimeBase::TB_NONE, 0, 0);
            stat = this->m_rcvdBuffer[packet].deserializeTo(recTimeTag);
            ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat);
            ASSERT_TRUE(timeTag == recTimeTag);

            U32 readVal;
            stat = this->m_rcvdBuffer[packet].deserializeTo(readVal);
            ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat);

            if (chanNum == currentChan) {
                ASSERT_EQ(id, sentId);
                ASSERT_EQ(val, readVal);
            }

            if (currentChan == (totalChan - 1)) {
                break;
            }

            currentChan++;
        }

        ASSERT_EQ(0, this->m_rcvdBuffer[packet].getDeserializeSizeLeft());
    }
}

void TlmChanTester::checkGuardEvent(U32 expectedDeferred, U32 expectedCapCount) {
    // Exactly one ProcCapReached event must have been emitted this run
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_TlmChanEpochProcessingCapReached_SIZE(1);

    // Verify both event parameters
    ASSERT_EVENTS_TlmChanEpochProcessingCapReached(0, expectedDeferred, expectedCapCount);
}

void TlmChanTester::sendBuff(FwChanIdType id, U32 val) {
    Fw::TlmBuffer buff;
    Fw::TlmBuffer readBack;
    Fw::SerializeStatus stat;
    Fw::Time timeTag;
    U32 retestVal;

    buff.resetSer();
    stat = buff.serializeFrom(val);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat);

    static bool tlc001 = false;

    if (not tlc001) {
        REQUIREMENT("TLC-001");
        tlc001 = true;
    }

    this->invoke_to_TlmRecv(0, id, timeTag, buff);

    static bool tlc002 = false;

    if (not tlc002) {
        REQUIREMENT("TLC-002");
        tlc002 = true;
    }

    Fw::TlmValid valid = this->invoke_to_TlmGet(0, id, timeTag, readBack);
    retestVal = 0;
    readBack.deserializeTo(retestVal);
    ASSERT_EQ(retestVal, val);
    ASSERT_EQ(valid, Fw::TlmValid::VALID);
}

void TlmChanTester::clearBuffs() {
    this->m_numBuffs = 0;
    for (FwChanIdType n = 0; n < TLMCHAN_HASH_BUCKETS; n++) {
        this->m_rcvdBuffer[n].resetSer();
    }
}

void TlmChanTester::dumpTlmEntry(TlmChan::TlmEntry* entry) {
    printf(
        "Entry "
        " Ptr: %p"
        " id: 0x%" PRI_FwChanIdType " bucket: %" PRI_FwChanIdType " next: %p\n",
        static_cast<void*>(entry), entry->id, entry->bucketNo, static_cast<void*>(entry->next));
}

void TlmChanTester::dumpHash() {
    for (FwChanIdType slot = 0; slot < TLMCHAN_NUM_TLM_HASH_SLOTS; slot++) {
        printf("Slot: %" PRI_FwChanIdType "\n", slot);
        if (this->component.m_tlmEntries[0].slots[slot]) {
            TlmChan::TlmEntry* entry = component.m_tlmEntries[0].slots[slot];
            for (FwChanIdType bucket = 0; bucket < TLMCHAN_HASH_BUCKETS; bucket++) {
                dumpTlmEntry(entry);
                if (entry->next == nullptr) {
                    break;
                } else {
                    entry = entry->next;
                }
            }
        } else {
            printf("EMPTY\n");
        }
    }
    printf("\n");
}

void TlmChanTester ::connectPorts() {
    // Run
    this->connect_to_Run(0, this->component.get_Run_InputPort(0));

    // TlmGet
    this->connect_to_TlmGet(0, this->component.get_TlmGet_InputPort(0));

    // TlmRecv
    this->connect_to_TlmRecv(0, this->component.get_TlmRecv_InputPort(0));

    // pingIn
    this->connect_to_pingIn(0, this->component.get_pingIn_InputPort(0));

    // PktSend
    this->component.set_PktSend_OutputPort(0, this->get_from_PktSend(0));

    // pingOut
    this->component.set_pingOut_OutputPort(0, this->get_from_pingOut(0));

    // Event infrastructure: wire time, event, and text event ports so that
    // log_WARNING_HI_ProcCapReached() is captured in the test event history.
    // Without these connections the event fires into unconnected ports and
    // ASSERT_EVENTS_ProcCapReached will always see an empty history.
    this->component.set_timeCaller_OutputPort(0, this->get_from_timeCaller(0));
    this->component.set_eventOut_OutputPort(0, this->get_from_eventOut(0));
}

void TlmChanTester ::initComponents() {
    this->init();
    this->component.init(QUEUE_DEPTH, INSTANCE);
}

}  // end namespace Svc
