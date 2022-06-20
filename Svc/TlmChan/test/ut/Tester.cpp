// ======================================================================
// \title  TlmChan.hpp
// \author tcanham
// \brief  cpp file for TlmChan test harness implementation class
// ======================================================================

#include "Tester.hpp"
#include <Fw/Test/UnitTest.hpp>

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10
#define QUEUE_DEPTH 10

static const NATIVE_UINT_TYPE TEST_CHAN_SIZE = sizeof(FwChanIdType) + Fw::Time::SERIALIZED_SIZE + sizeof(U32);
static const NATIVE_UINT_TYPE CHANS_PER_COMBUFFER = (FW_COM_BUFFER_MAX_SIZE - sizeof(FwPacketDescriptorType))/TEST_CHAN_SIZE;

namespace Svc
{

    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    Tester ::
        Tester() : TlmChanGTestBase("Tester", MAX_HISTORY_SIZE),
                   component("TlmChan")
        ,m_numBuffs(0)
        , m_bufferRecv(false)
    {
        this->initComponents();
        this->connectPorts();
    }

    Tester ::
        ~Tester()
    {
    }

    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void Tester::runNominalChannel()
    {

        this->clearBuffs();
        // send first buffer
        this->sendBuff(27, 10, 0);
        this->doRun(true);
        this->checkBuff(27, 10, 0);

        this->clearBuffs();
        // send again to other buffer
        this->sendBuff(27, 10, 0);

        static bool tlc003 = false;

        if (not tlc003)
        {
            REQUIREMENT("TLC-003");
            tlc003 = true;
        }

        this->doRun(true);
        this->checkBuff(27, 10, 0);

        // do an update to make sure it gets updated and returned correctly
        this->clearBuffs();
        this->sendBuff(27, 20, 0);
    }

    void Tester::runMultiChannel() {


        FwChanIdType IDs[] = {
                // Test channel IDs
                0x1000,0x1001,0x1002,0x1003,0x1004,0x1005,0x1100,0x1101,0x1102,0x1103,0x300,0x301,0x400,0x401,0x402,0x100,0x101,0x102,0x103,0x104,0x105
        };

        this->clearBuffs();
        // send all updates
        for (NATIVE_UINT_TYPE n=0; n < FW_NUM_ARRAY_ELEMENTS(IDs); n++) {
            this->sendBuff(IDs[n],n,0);
        }

        // dump hash table
        //this->dumpHash();

        // do a run, and all the packets should be sent
        this->doRun(true);
        ASSERT_TRUE(this->m_bufferRecv);
        ASSERT_EQ((FW_NUM_ARRAY_ELEMENTS(IDs)/CHANS_PER_COMBUFFER)+1,this->m_numBuffs);

        // verify packets
        for (NATIVE_UINT_TYPE n=0; n < FW_NUM_ARRAY_ELEMENTS(IDs); n++) {
            //printf("#: %d\n",n);
            this->checkBuff(IDs[n],n,0);
        }

    }


    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    void Tester ::
        from_PktSend_handler(
            const NATIVE_INT_TYPE portNum,
            Fw::ComBuffer &data,
            U32 context)
    {
        this->pushFromPortEntry_PktSend(data, context);
        this->m_bufferRecv = true;
        this->m_rcvdBuffer[this->m_numBuffs] = data;
        this->m_numBuffs++;
    }

    void Tester ::
        from_pingOut_handler(
            const NATIVE_INT_TYPE portNum,
            U32 key)
    {
        this->pushFromPortEntry_pingOut(key);
    }

    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    bool Tester::doRun(bool check) {
        // execute run port to send packet
        this->invoke_to_Run(0,0);
        // dispatch run message
        this->m_bufferRecv = false;
        this->component.doDispatch();
        if (check) {
            EXPECT_TRUE(this->m_bufferRecv);
        }
        return this->m_bufferRecv;
    }

    void Tester::checkBuff(FwChanIdType id, U32 val, NATIVE_INT_TYPE instance) {
        Fw::Time timeTag;
        // deserialize packet
        Fw::SerializeStatus stat;
        bool packetFound = false;

        static bool tlc004 = false;

        if (not tlc004) {
            REQUIREMENT("TLC-004");
            tlc004 = true;
        }

        // Search for channel ID
        for (NATIVE_UINT_TYPE packet = 0; packet < this->m_numBuffs; packet++) {
            // check for telemetry packet in ComBuffers
            this->m_rcvdBuffer[packet].resetDeser();
            // first piece should be tlm packet descriptor
            FwPacketDescriptorType desc;
            stat = this->m_rcvdBuffer[packet].deserialize(desc);
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
            ASSERT_EQ(desc, static_cast<FwPacketDescriptorType>(Fw::ComPacket::FW_PACKET_TELEM));
continue;            
            // next piece should be event ID
            FwEventIdType sentId;
            stat = this->m_rcvdBuffer[packet].deserialize(sentId);
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);

            // this is the packet you are looking for..
            if (sentId != id) {
                this->m_rcvdBuffer[packet].resetDeser();
                continue;
            }
            packetFound = true;
            // next piece is time tag
            Fw::Time recTimeTag(TB_NONE,0,0);
            stat = this->m_rcvdBuffer[packet].deserialize(recTimeTag);
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
            ASSERT_TRUE(timeTag == recTimeTag);
            // next piece is event argument
            U32 readVal;
            stat = this->m_rcvdBuffer[packet].deserialize(readVal);
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
            ASSERT_EQ(readVal, val);
            // packet should be empty
            ASSERT_EQ(this->m_rcvdBuffer[packet].getBuffLeft(),0u);
        }

        ASSERT_TRUE(packetFound);
    }

    void Tester::sendBuff(FwChanIdType id, U32 val, NATIVE_INT_TYPE instance) {

        Fw::TlmBuffer buff;
        Fw::TlmBuffer readBack;
        Fw::SerializeStatus stat;
        Fw::Time timeTag;
        U32 retestVal;

        // create telemetry item
        buff.resetSer();
        stat = buff.serialize(val);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);

        static bool tlc001 = false;

        if (not tlc001) {
            REQUIREMENT("TLC-001");
            tlc001 = true;
        }

        this->invoke_to_TlmRecv(0,id,timeTag,buff);
        // Read back value
        static bool tlc002 = false;

        if (not tlc002) {
            REQUIREMENT("TLC-002");
            tlc002 = true;
        }

        this->invoke_to_TlmGet(0,id,timeTag,readBack);
        // deserialize value
        retestVal = 0;
        readBack.deserialize(retestVal);
        ASSERT_EQ(retestVal, val);


    }

    void Tester::clearBuffs() {
        this->m_numBuffs = 0;
        for (NATIVE_INT_TYPE n = 0; n < TLMCHAN_HASH_BUCKETS; n++) {
            this->m_rcvdBuffer[n].resetSer();
        }
    }

    void Tester::dumpTlmEntry(TlmChanImpl::TlmEntry* entry) {
        printf(
                "Entry "
                " Ptr: %p"
                " id: 0x%08X"
                " bucket: %d"
                " next: %p\n",
                static_cast<void *>(entry),entry->id,entry->bucketNo,static_cast<void *>(entry->next)
                );
    }

    void Tester::dumpHash() {
//        printf("**Buffer 0\n");
        for (NATIVE_INT_TYPE slot = 0; slot < TLMCHAN_NUM_TLM_HASH_SLOTS; slot++) {
            printf("Slot: %d\n",slot);
            if (this->component.m_tlmEntries[0].slots[slot]) {
                TlmChanImpl::TlmEntry* entry = component.m_tlmEntries[0].slots[slot];
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


    void Tester ::
        connectPorts()
    {

        // Run
        this->connect_to_Run(
            0,
            this->component.get_Run_InputPort(0));

        // TlmGet
        this->connect_to_TlmGet(
            0,
            this->component.get_TlmGet_InputPort(0));

        // TlmRecv
        this->connect_to_TlmRecv(
            0,
            this->component.get_TlmRecv_InputPort(0));

        // pingIn
        this->connect_to_pingIn(
            0,
            this->component.get_pingIn_InputPort(0));

        // PktSend
        this->component.set_PktSend_OutputPort(
            0,
            this->get_from_PktSend(0));

        // pingOut
        this->component.set_pingOut_OutputPort(
            0,
            this->get_from_pingOut(0));
    }

    void Tester ::
        initComponents()
    {
        this->init();
        this->component.init(
            QUEUE_DEPTH, INSTANCE);
    }

} // end namespace Svc
