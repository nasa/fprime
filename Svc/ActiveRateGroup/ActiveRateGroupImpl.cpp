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

#include <Svc/ActiveRateGroup/ActiveRateGroupImpl.hpp>
#include <ActiveRateGroupImplCfg.hpp>
#include <FpConfig.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/Log.hpp>

namespace Svc {

    ActiveRateGroupImpl::ActiveRateGroupImpl(const char* compName, NATIVE_UINT_TYPE contexts[], NATIVE_UINT_TYPE numContexts) :
            ActiveRateGroupComponentBase(compName),
            m_cycles(0),
            m_maxTime(0),
            m_cycleStarted(false),
            m_overrunThrottle(0),
            m_cycleSlips(0) {
        FW_ASSERT(contexts);
        FW_ASSERT(numContexts == static_cast<NATIVE_UINT_TYPE>(this->getNum_RateGroupMemberOut_OutputPorts()),numContexts,this->getNum_RateGroupMemberOut_OutputPorts());
        FW_ASSERT(FW_NUM_ARRAY_ELEMENTS(this->m_contexts) == this->getNum_RateGroupMemberOut_OutputPorts(),
                static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_contexts)),
                this->getNum_RateGroupMemberOut_OutputPorts());

        // copy context values
        for (NATIVE_INT_TYPE entry = 0; entry < this->getNum_RateGroupMemberOut_OutputPorts(); entry++) {
            this->m_contexts[entry] = contexts[entry];
        }
    }

    void ActiveRateGroupImpl::init(NATIVE_INT_TYPE queueDepth, NATIVE_INT_TYPE instance) {
        ActiveRateGroupComponentBase::init(queueDepth,instance);
    }

    ActiveRateGroupImpl::~ActiveRateGroupImpl() {

    }

    void ActiveRateGroupImpl::preamble() {
        this->log_DIAGNOSTIC_RateGroupStarted();
    }

    void ActiveRateGroupImpl::CycleIn_handler(NATIVE_INT_TYPE portNum, Svc::TimerVal& cycleStart) {

        TimerVal end;

        this->m_cycleStarted = false;

        // invoke any members of the rate group
        for (NATIVE_INT_TYPE port = 0; port < this->getNum_RateGroupMemberOut_OutputPorts(); port++) {
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

    void ActiveRateGroupImpl::CycleIn_preMsgHook(NATIVE_INT_TYPE portNum, Svc::TimerVal& cycleStart) {
        // set flag to indicate cycle has started. Check in thread for overflow.
        this->m_cycleStarted = true;
    }

    void ActiveRateGroupImpl::PingIn_handler(NATIVE_INT_TYPE portNum, U32 key) {
        // return the key to health
        this->PingOut_out(0,key);
    }


}
