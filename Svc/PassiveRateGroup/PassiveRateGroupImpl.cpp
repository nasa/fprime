/*
* \author: Tim Canham
* \file:
* \brief
*
* This file implements the PassiveRateGroup component,
* which invokes a set of components the comprise the rate group.
*
*   Copyright 2014-2015, by the California Institute of Technology.
*   ALL RIGHTS RESERVED. United States Government Sponsorship
*   acknowledged. Any commercial use must be negotiated with the Office
*   of Technology Transfer at the California Institute of Technology.
*/

#include <Svc/PassiveRateGroup/PassiveRateGroupImpl.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/Log.hpp>

namespace Svc {

#if FW_OBJECT_NAMES == 1
    PassiveRateGroupImpl::PassiveRateGroupImpl(const char* compName, NATIVE_UINT_TYPE contexts[], NATIVE_UINT_TYPE numContexts) :
        PassiveRateGroupComponentBase(compName),
#else
    PassiveRateGroupImpl::PassiveRateGroupImpl(NATIVE_UINT_TYPE contexts[], NATIVE_UINT_TYPE numContexts) :
#endif
        m_cycles(0), m_maxTime(0) {

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
    
    void PassiveRateGroupImpl::init(NATIVE_INT_TYPE instance) {
        PassiveRateGroupComponentBase::init(instance);
    }

    PassiveRateGroupImpl::~PassiveRateGroupImpl(void) {
    }

    void PassiveRateGroupImpl::CycleIn_handler(NATIVE_INT_TYPE portNum, Svc::TimerVal& cycleStart) {

        TimerVal end;

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
    }

}
