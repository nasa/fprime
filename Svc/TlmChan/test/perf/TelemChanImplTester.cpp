/*
 * PrmDbImplTester.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#include <Svc/TlmChan/test/ut/TelemChanImplTester.hpp>
#include <Fw/Com/FwComBuffer.hpp>
#include <Fw/Com/FwComPacket.hpp>
#include <Os/IntervalTimer.hpp>

#include <cstdio>


namespace Svc {

    TelemStoreComponentBaseFriend::TelemStoreComponentBaseFriend(Svc::TelemStoreComponentBase& inst) : m_baseInst(inst) {
    }

    Fw::QueuedComponentBase::MsgDispatchStatus TelemStoreComponentBaseFriend::doDispatch(void) {
        return this->m_baseInst.doDispatch();
    }

    void TelemChanImplTester::init(NATIVE_INT_TYPE instance) {
        Svc::TelemStoreComponentTesterBase::init();
    }

    void TelemChanImplTester::pktSend_handler(NATIVE_INT_TYPE portNum, Fw::ComBuffer &data) {

    }

    TelemChanImplTester::TelemChanImplTester(Svc::TelemChanImpl& inst) : Svc::TelemStoreComponentTesterBase("testerbase"),m_impl(inst), m_baseFriend(inst) {

    }

    TelemChanImplTester::~TelemChanImplTester() {
    }

    void TelemChanImplTester::doPerfTest(U32 iterations) {

        Os::IntervalTimer timer;
        Fw::TlmBuffer buff;
        Fw::TlmBuffer readBack;
        Fw::SerializeStatus stat;
        Fw::Time timeTag;

        U32 testVal = 10;
        U32 retestVal = 0;

        // prefill telemetry to force a linear search to the end

        for (U32 entry = 0; entry < TelemChanImpl::NUM_TLM_ENTRIES - 1; entry++) {
            Fw::TlmBuffer fakeBuff;
            this->tlmRecv_out(0,entry,timeTag,buff);
        }

        timer.start();

        for (U32 iter = 0; iter < iterations; iter++) {
            // create telemetry item
            buff.resetSer();
            stat = buff.serialize(testVal);
            this->tlmRecv_out(0,TelemChanImpl::NUM_TLM_ENTRIES - 1,timeTag,buff);
            // Read back value
            this->tlmGet_out(0,TelemChanImpl::NUM_TLM_ENTRIES - 1,timeTag,readBack);
            // deserialize value
            retestVal = 0;
            buff.deserialize(retestVal);
            if (retestVal != testVal) {
                printf("Mismatch: %d %d\n",testVal,retestVal);
                break;
            }
        }

        timer.stop();
        printf("Write total: %d ave: %d\n",timer.getDiffUsec(),timer.getDiffUsec()/iterations);

    }

} /* namespace SvcTest */
