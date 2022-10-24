/*
* \author: Tim Canham
* \file:
* \brief
*
* This file implements the ActiveRateGroup component,
* which invokes a set of components the comprise the rate group.
*
*   Copyright 2014-2015, by the California Institute of Technology.
*   ALL RIGHTS RESERVED. United States Government Sponsorship
*   acknowledged.
*
*/

#include <Svc/ActiveRateGroup/ActiveRateGroup.hpp>
#include <ActiveRateGroupCfg.hpp>
#include <FpConfig.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/Log.hpp>

namespace Svc {

    ActiveRateGroup::ActiveRateGroup(const char* compName) :
            ActiveRateGroupComponentBase(compName),
            m_cycles(0),
            m_maxTime(0),
            m_cycleStarted(false),
            m_numContexts(0),
            m_overrunThrottle(0),
            m_cycleSlips(0) {
    }

    void ActiveRateGroup::configure( NATIVE_INT_TYPE contexts[], NATIVE_INT_TYPE numContexts) {
        FW_ASSERT(contexts);
        FW_ASSERT(numContexts == this->getNum_RateGroupMemberOut_OutputPorts(),numContexts,this->getNum_RateGroupMemberOut_OutputPorts());
        FW_ASSERT(FW_NUM_ARRAY_ELEMENTS(this->m_contexts) == this->getNum_RateGroupMemberOut_OutputPorts(),
                FW_NUM_ARRAY_ELEMENTS(this->m_contexts),
                this->getNum_RateGroupMemberOut_OutputPorts());

        this->m_numContexts = numContexts;
        // copy context values
        for (NATIVE_INT_TYPE entry = 0; entry < this->m_numContexts; entry++) {
            this->m_contexts[entry] = contexts[entry];
        }
    }

    void ActiveRateGroup::init(NATIVE_INT_TYPE queueDepth, NATIVE_INT_TYPE instance) {
        ActiveRateGroupComponentBase::init(queueDepth,instance);
    }

    ActiveRateGroup::~ActiveRateGroup() {

    }

    void ActiveRateGroup::preamble() {
        this->log_DIAGNOSTIC_RateGroupStarted();
    }

    void ActiveRateGroup::CycleIn_handler(NATIVE_INT_TYPE portNum, Svc::TimerVal& cycleStart) {

        // Make sure it's been configured
        FW_ASSERT(this->m_numContexts);

        TimerVal end;

        this->m_cycleStarted = false;

        // invoke any members of the rate group
        for (NATIVE_INT_TYPE port = 0; port < this->m_numContexts; port++) {
            if (this->isConnected_RateGroupMemberOut_OutputPort(port)) {
                this->RateGroupMemberOut_out(port,this->m_contexts[port]);
            }
        }

        // grab timer for end of cycle
        end.take();

        // get rate group execution time
        U32 cycle_time = end.diffUSec(cycleStart);

        // check to see if the time has exceeded the previous maximum
        if (cycle_time > this->m_maxTime) {
            this->m_maxTime = cycle_time;
        }

        // update cycle telemetry
        this->tlmWrite_RgMaxTime(this->m_maxTime);

        // check for cycle slip. That will happen if new cycle message has been received
        // which will cause flag will be set again.
        if (this->m_cycleStarted) {
            this->m_cycleSlips++;
            if (this->m_overrunThrottle < ACTIVE_RATE_GROUP_OVERRUN_THROTTLE) {
                this->log_WARNING_HI_RateGroupCycleSlip(this->m_cycles);
                this->m_overrunThrottle++;
            }
            // update cycle slips
            this->tlmWrite_RgCycleSlips(this->m_cycleSlips);
        } else { // if cycle is okay start decrementing throttle value
            if (this->m_overrunThrottle > 0) {
                this->m_overrunThrottle--;
            }
        }

        // increment cycle
        this->m_cycles++;

    }

    void ActiveRateGroup::CycleIn_preMsgHook(NATIVE_INT_TYPE portNum, Svc::TimerVal& cycleStart) {
        // set flag to indicate cycle has started. Check in thread for overflow.
        this->m_cycleStarted = true;
    }

    void ActiveRateGroup::PingIn_handler(NATIVE_INT_TYPE portNum, U32 key) {
        // return the key to health
        this->PingOut_out(0,key);
    }


}
