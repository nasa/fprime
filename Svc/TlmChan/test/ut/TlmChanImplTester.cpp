/*
 * PrmDbImplTester.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#include <Svc/TlmChan/test/ut/TlmChanImplTester.hpp>
#include <Fw/Com/ComBuffer.hpp>
#include <Fw/Com/ComPacket.hpp>
#include <Os/IntervalTimer.hpp>
#include <Fw/Test/UnitTest.hpp>

#include <cstdio>

#include <gtest/gtest.h>


namespace Svc {

    void TlmChanImplTester::init(NATIVE_INT_TYPE instance) {
        TlmChanGTestBase::init();
    }

    void TlmChanImplTester::from_PktSend_handler(NATIVE_INT_TYPE portNum, Fw::ComBuffer &data, U32 context) {
        this->m_bufferRecv = true;
        this->m_rcvdBuffer[this->m_numBuffs] = data;
        this->m_numBuffs++;
    }

    TlmChanImplTester::TlmChanImplTester(Svc::TlmChanImpl& inst) :
        TlmChanGTestBase("testerbase",100),
            m_impl(inst),
            m_numBuffs(0),
            m_bufferRecv(false) {
    }

    TlmChanImplTester::~TlmChanImplTester() {
    }

    bool TlmChanImplTester::doRun(bool check) {
        // execute run port to send packet
        this->invoke_to_Run(0,0);
        // dispatch run message
        this->m_bufferRecv = false;
        this->m_impl.doDispatch();
        if (check) {
            EXPECT_TRUE(this->m_bufferRecv);
        }
        return this->m_bufferRecv;
    }

    void TlmChanImplTester::checkBuff(FwChanIdType id, U32 val, NATIVE_INT_TYPE instance) {
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
            this->m_rcvdBuffer[packet].resetDeser();
            // first piece should be tlm packet descriptor
            FwPacketDescriptorType desc;
            stat = this->m_rcvdBuffer[packet].deserialize(desc);
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
            ASSERT_EQ(desc,(FwPacketDescriptorType)Fw::ComPacket::FW_PACKET_TELEM);
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
            ASSERT_EQ(this->m_rcvdBuffer[packet].getBuffLeft(),(NATIVE_UINT_TYPE)0);
        }

        ASSERT_TRUE(packetFound);
    }

    void TlmChanImplTester::sendBuff(FwChanIdType id, U32 val, NATIVE_INT_TYPE instance) {

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

    void TlmChanImplTester::runNominalChannel(void) {

        this->clearBuffs();
        // send first buffer
        this->sendBuff(27,10,0);
        this->doRun(true);
        this->checkBuff(27,10,0);

        this->clearBuffs();
        // send again to other buffer
        this->sendBuff(27,10,0);

        static bool tlc003 = false;

        if (not tlc003) {
            REQUIREMENT("TLC-003");
            tlc003 = true;
        }

        this->doRun(true);
        this->checkBuff(27,10,0);

        // do an update to make sure it gets updated and returned correctly
        this->clearBuffs();
        this->sendBuff(27,20,0);


    }

    void TlmChanImplTester::runMultiChannel(void) {


        FwChanIdType IDs[] = {
                // From Ref ComponentReport.txt
                0x1000,0x1001,0x1002,0x1003,0x1004,0x1005,0x1100,0x1101,0x1102,0x1103,0x300,0x301,0x400,0x401,0x402,0x100,0x101,0x102,0x103,0x104,0x105
        };

        this->clearBuffs();
        // send all updates
        for (NATIVE_UINT_TYPE n=0; n < FW_NUM_ARRAY_ELEMENTS(IDs); n++) {
            this->sendBuff(IDs[n],n,0);
        }

        // dump hash table
        this->dumpHash();

        // do a run, and all the packets should be sent
        this->doRun(true);
        ASSERT_TRUE(this->m_bufferRecv);
        ASSERT_EQ(this->m_numBuffs,FW_NUM_ARRAY_ELEMENTS(IDs));

        // verify packets

        for (NATIVE_UINT_TYPE n=0; n < FW_NUM_ARRAY_ELEMENTS(IDs); n++) {
            //printf("#: %d\n",n);
            this->checkBuff(IDs[n],n,0);
        }

    }

    void TlmChanImplTester::runTooManyChannels(void) {

        // This will assert, so disable after testing

        this->clearBuffs();
        // send all updates
//        for (NATIVE_UINT_TYPE n=0; n <= TLMCHAN_HASH_BUCKETS; n++) {
        for (NATIVE_UINT_TYPE n=0; n < TLMCHAN_HASH_BUCKETS; n++) {
            this->sendBuff(n,n,0);
        }


    }

    void TlmChanImplTester::runOffNominal(void) {

        // Ask for a packet that isn't written yet
        Fw::TlmBuffer buff;
        Fw::SerializeStatus stat;
        Fw::Time timeTag;
        U32 val = 10;

        // create Tlmtry item and put dummy data in to make sure it gets erased
        buff.resetSer();
        stat = buff.serialize(val);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);

        // Read back value
        this->invoke_to_TlmGet(0,10,timeTag,buff);
        ASSERT_EQ((NATIVE_UINT_TYPE)0,buff.getBuffLength());

    }

    void TlmChanImplTester::clearBuffs(void) {
        this->m_numBuffs = 0;
        for (NATIVE_INT_TYPE n = 0; n < TLMCHAN_HASH_BUCKETS; n++) {
            this->m_rcvdBuffer[n].resetSer();
        }
    }

    void TlmChanImplTester::dumpTlmEntry(TlmChanImpl::TlmEntry* entry) {
        printf(
                "Entry "
                " Ptr: %p"
                " id: 0x%08X"
                " bucket: %d"
                " next: %p\n",
                entry,entry->id,entry->bucketNo,entry->next
                );
    }

    void TlmChanImplTester::dumpHash(void) {
//        printf("**Buffer 0\n");
        for (NATIVE_INT_TYPE slot = 0; slot < TLMCHAN_NUM_TLM_HASH_SLOTS; slot++) {
            printf("Slot: %d\n",slot);
            if (m_impl.m_tlmEntries[0].slots[slot]) {
                TlmChanImpl::TlmEntry* entry = m_impl.m_tlmEntries[0].slots[slot];
                for (NATIVE_INT_TYPE bucket = 0; bucket < TLMCHAN_HASH_BUCKETS; bucket++) {
                    dumpTlmEntry(entry);
                    if (entry->next == 0) {
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
    void TlmChanImplTester ::
      from_pingOut_handler(
          const NATIVE_INT_TYPE portNum,
          U32 key
      )
    {
      this->pushFromPortEntry_pingOut(key);
    }
} /* namespace SvcTest */
