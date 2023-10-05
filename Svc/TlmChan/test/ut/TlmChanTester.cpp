// ======================================================================
// \title  TlmChan.hpp
// \author tcanham
// \brief  cpp file for TlmChan test harness implementation class
// ======================================================================

#include "TlmChanTester.hpp"
#include <Fw/Test/UnitTest.hpp>

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10
#define QUEUE_DEPTH 10

static const NATIVE_UINT_TYPE TEST_CHAN_SIZE = sizeof(FwChanIdType) + Fw::Time::SERIALIZED_SIZE + sizeof(U32);
static const NATIVE_UINT_TYPE CHANS_PER_COMBUFFER =
    (FW_COM_BUFFER_MAX_SIZE - sizeof(FwPacketDescriptorType)) / TEST_CHAN_SIZE;

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

TlmChanTester ::TlmChanTester()
    : TlmChanGTestBase("Tester", MAX_HISTORY_SIZE), component("TlmChan"), m_numBuffs(0), m_bufferRecv(false) {
    this->initComponents();
    this->connectPorts();
}

TlmChanTester ::~TlmChanTester() {}

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
    for (NATIVE_UINT_TYPE n = 0; n < FW_NUM_ARRAY_ELEMENTS(ID_0); n++) {
        this->sendBuff(ID_0[n], n);
    }

    ASSERT_EQ(0, this->component.m_activeBuffer);

    // do a run, and all the packets should be sent
    this->doRun(true);
    ASSERT_TRUE(this->m_bufferRecv);
    ASSERT_EQ((FW_NUM_ARRAY_ELEMENTS(ID_0) / CHANS_PER_COMBUFFER) + 1, this->m_numBuffs);
    ASSERT_EQ(1, this->component.m_activeBuffer);

    // verify packets
    for (NATIVE_UINT_TYPE n = 0; n < FW_NUM_ARRAY_ELEMENTS(ID_0); n++) {
        // printf("#: %d\n",n);
        this->checkBuff(n, FW_NUM_ARRAY_ELEMENTS(ID_0), ID_0[n], n);
    }

    // send another set

    FwChanIdType ID_1[] = {// Test channel IDs
                           0x5000, 0x5001, 0x5002, 0x5003, 0x5004, 0x5005, 0x5100, 0x5101, 0x5102,
                           0x5103, 0x6300, 0x6301, 0x6400, 0x6401, 0x6402, 0x6100, 0x6101, 0x6102,
                           0x6103, 0x6104, 0x6105, 0x8101, 0x8102, 0x8103, 0x8104, 0x8105};

    this->clearBuffs();
    // send all updates
    for (NATIVE_UINT_TYPE n = 0; n < FW_NUM_ARRAY_ELEMENTS(ID_1); n++) {
        this->sendBuff(ID_1[n], n);
    }

    ASSERT_EQ(1, this->component.m_activeBuffer);

    // do a run, and all the packets should be sent
    this->doRun(true);
    ASSERT_TRUE(this->m_bufferRecv);
    ASSERT_EQ((FW_NUM_ARRAY_ELEMENTS(ID_1) / CHANS_PER_COMBUFFER) + 1, this->m_numBuffs);
    ASSERT_EQ(0, this->component.m_activeBuffer);

    // verify packets
    for (NATIVE_UINT_TYPE n = 0; n < FW_NUM_ARRAY_ELEMENTS(ID_1); n++) {
        // printf("#: %d\n",n);
        this->checkBuff(n, FW_NUM_ARRAY_ELEMENTS(ID_1), ID_1[n], n);
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
    stat = buff.serialize(val);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat);

    // Read back value
    this->invoke_to_TlmGet(0, 10, timeTag, buff);
    ASSERT_EQ(0u, buff.getBuffLength());
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void TlmChanTester ::from_PktSend_handler(const NATIVE_INT_TYPE portNum, Fw::ComBuffer& data, U32 context) {
    this->pushFromPortEntry_PktSend(data, context);
    this->m_bufferRecv = true;
    this->m_rcvdBuffer[this->m_numBuffs] = data;
    this->m_numBuffs++;
}

void TlmChanTester ::from_pingOut_handler(const NATIVE_INT_TYPE portNum, U32 key) {
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

void TlmChanTester::checkBuff(NATIVE_UINT_TYPE chanNum, NATIVE_UINT_TYPE totalChan, FwChanIdType id, U32 val) {
    Fw::Time timeTag;
    // deserialize packet
    Fw::SerializeStatus stat;

    static bool tlc004 = false;

    if (not tlc004) {
        REQUIREMENT("TLC-004");
        tlc004 = true;
    }

    NATIVE_UINT_TYPE currentChan = 0;

    // Search for channel ID
    for (NATIVE_UINT_TYPE packet = 0; packet < this->m_numBuffs; packet++) {
        // Look at packet descriptor for current packet
        this->m_rcvdBuffer[packet].resetDeser();
        // first piece should be tlm packet descriptor
        FwPacketDescriptorType desc;
        stat = this->m_rcvdBuffer[packet].deserialize(desc);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat);
        ASSERT_EQ(desc, static_cast<FwPacketDescriptorType>(Fw::ComPacket::FW_PACKET_TELEM));

        for (NATIVE_UINT_TYPE chan = 0; chan < CHANS_PER_COMBUFFER; chan++) {
            // decode channel ID
            FwEventIdType sentId;
            stat = this->m_rcvdBuffer[packet].deserialize(sentId);
            ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat);

            // next piece is time tag
            Fw::Time recTimeTag(TB_NONE, 0, 0);
            stat = this->m_rcvdBuffer[packet].deserialize(recTimeTag);
            ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat);
            ASSERT_TRUE(timeTag == recTimeTag);
            // next piece is event argument
            U32 readVal;
            stat = this->m_rcvdBuffer[packet].deserialize(readVal);
            ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat);

            if (chanNum == currentChan) {
                ASSERT_EQ(id, sentId);
                ASSERT_EQ(val, readVal);
            }

            // quit if we are at max channel entry
            if (currentChan == (totalChan - 1)) {
                break;
            }

            currentChan++;
        }

        // packet should be empty
        ASSERT_EQ(0, this->m_rcvdBuffer[packet].getBuffLeft());
    }
}

void TlmChanTester::sendBuff(FwChanIdType id, U32 val) {
    Fw::TlmBuffer buff;
    Fw::TlmBuffer readBack;
    Fw::SerializeStatus stat;
    Fw::Time timeTag;
    U32 retestVal;

    // create telemetry item
    buff.resetSer();
    stat = buff.serialize(val);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat);

    static bool tlc001 = false;

    if (not tlc001) {
        REQUIREMENT("TLC-001");
        tlc001 = true;
    }

    this->invoke_to_TlmRecv(0, id, timeTag, buff);
    // Read back value
    static bool tlc002 = false;

    if (not tlc002) {
        REQUIREMENT("TLC-002");
        tlc002 = true;
    }

    this->invoke_to_TlmGet(0, id, timeTag, readBack);
    // deserialize value
    retestVal = 0;
    readBack.deserialize(retestVal);
    ASSERT_EQ(retestVal, val);
}

void TlmChanTester::clearBuffs() {
    this->m_numBuffs = 0;
    for (NATIVE_INT_TYPE n = 0; n < TLMCHAN_HASH_BUCKETS; n++) {
        this->m_rcvdBuffer[n].resetSer();
    }
}

void TlmChanTester::dumpTlmEntry(TlmChan::TlmEntry* entry) {
    printf(
        "Entry "
        " Ptr: %p"
        " id: 0x%08X"
        " bucket: %d"
        " next: %p\n",
        static_cast<void*>(entry), entry->id, entry->bucketNo, static_cast<void*>(entry->next));
}

void TlmChanTester::dumpHash() {
    //        printf("**Buffer 0\n");
    for (NATIVE_INT_TYPE slot = 0; slot < TLMCHAN_NUM_TLM_HASH_SLOTS; slot++) {
        printf("Slot: %d\n", slot);
        if (this->component.m_tlmEntries[0].slots[slot]) {
            TlmChan::TlmEntry* entry = component.m_tlmEntries[0].slots[slot];
            for (NATIVE_INT_TYPE bucket = 0; bucket < TLMCHAN_HASH_BUCKETS; bucket++) {
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
    //        for (NATIVE_INT_TYPE bucket = 0; bucket < TLMCHAN_HASH_BUCKETS; bucket++) {
    //            printf("Bucket: %d ",bucket);
    //            dumpTlmEntry(&m_impl.m_tlmEntries[0].buckets[bucket]);
    //        }
    //        printf("**Buffer 1\n");
    //        for (NATIVE_INT_TYPE slot = 0; slot < TLMCHAN_NUM_TLM_HASH_SLOTS; slot++) {
    //            printf("Slot: %d\n",slot);
    //            if (m_impl.m_tlmEntries[1].slots[slot]) {
    //                TlmChanImpl::TlmEntry* entry = m_impl.m_tlmEntries[1].slots[slot];
    //                for (NATIVE_INT_TYPE bucket = 0; bucket < TLMCHAN_HASH_BUCKETS; bucket++) {
    //                    dumpTlmEntry(entry);
    //                    if (entry->next == 0) {
    //                        break;
    //                    } else {
    //                        entry = entry->next;
    //                    }
    //                }
    //            } else {
    //                printf("EMPTY\n");
    //            }
    //        }
    //        printf("\n");
    //        for (NATIVE_INT_TYPE bucket = 0; bucket < TLMCHAN_HASH_BUCKETS; bucket++) {
    //            printf("Bucket: %d\n",bucket);
    //            dumpTlmEntry(&m_impl.m_tlmEntries[1].buckets[bucket]);
    //        }
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
}

void TlmChanTester ::initComponents() {
    this->init();
    this->component.init(QUEUE_DEPTH, INSTANCE);
}

}  // end namespace Svc
