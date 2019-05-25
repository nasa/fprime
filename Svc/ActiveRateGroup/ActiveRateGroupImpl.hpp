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

#ifndef SVC_ACTIVERATEGROUP_IMPL_HPP
#define SVC_ACTIVERATEGROUP_IMPL_HPP

#include <Svc/ActiveRateGroup/ActiveRateGroupComponentAc.hpp>

namespace Svc {

    //! \class ActiveRateGroupImpl
    //! \brief Executes a set of components as part of a rate group
    //!
    //! ActiveRateGroup takes an input cycle call to begin the rate group cycle.
    //! It calls each output port in succession and passes the value in the context
    //! array at the index corresponding to the output port number. It keeps track of the execution
    //! time of the rate group and detects overruns.
    //!

    class ActiveRateGroupImpl : public ActiveRateGroupComponentBase {
        public:

            //!  \brief ActiveRateGroupImpl constructor
            //!
            //!  The constructor of the class clears all the flags and copies the
            //!  contents of the context array to private storage.
            //!
            //!  \param compName Name of the component
            //!  \param contexts Array of integers that contain the context values that will be sent
            //!         to each member component. The index of the array corresponds to the
            //!         output port number.
            //!  \param numContexts The number of elements in the context array.

#if FW_OBJECT_NAMES == 1        
            ActiveRateGroupImpl(const char* compName, NATIVE_UINT_TYPE contexts[], NATIVE_UINT_TYPE numContexts);
#else
            ActiveRateGroupImpl(NATIVE_UINT_TYPE contexts[], NATIVE_UINT_TYPE numContexts);
#endif
      
            //!  \brief ActiveRateGroupImpl initialization function
            //!
            //!  The initialization function of the class initializes the member
            //!  ports and the component base class
            //!
            //!  \param queueDepth Depth of the active component message queue
            //!  \param instance Identifies the instance of the rate group component

            void init(NATIVE_INT_TYPE queueDepth, NATIVE_INT_TYPE instance);

            //!  \brief ActiveRateGroupImpl destructor
            //!
            //!  The destructor of the class is empty

            ~ActiveRateGroupImpl(void);

        PRIVATE:

            //!  \brief Input cycle port handler
            //!
            //!  The cycle port handler calls each component in the rate group in turn,
            //!  passing the context value. It computes the execution time each cycle,
            //!  and writes it to a telemetry value if it reaches a maximum time
            //!
            //!  \param portNum incoming port call. For this class, should always be zero
            //!  \param cycleStart value stored by the cycle driver, used to compute execution time.

            void CycleIn_handler(NATIVE_INT_TYPE portNum, Svc::TimerVal& cycleStart);

            //!  \brief Input cycle port pre message hook
            //!
            //!  The input cycle port pre message hook is called on the thread of the calling
            //!  cycle port. It sets flag to indicate that the cycle has started.
            //!
            //!  \param portNum incoming port call. For this class, should always be zero
            //!  \param cycleStart value stored by the cycle driver, used to compute execution time.

            void CycleIn_preMsgHook(NATIVE_INT_TYPE portNum, Svc::TimerVal& cycleStart); //!< CycleIn pre-message hook

            //!  \brief Input ping port handler
            //!
            //!  This port is called by the health task to verify task aliveness
            //!
            //!  \param portNum incoming port call. For this class, should always be zero
            //!  \param key value returned to health task to verify round trip

            void PingIn_handler(NATIVE_INT_TYPE portNum, U32 key);

            //!  \brief Task preamble
            //!
            //!  This method is called prior to entering the message loop.
            //!  It issues and event indicating that the task has started.
            //!

            void preamble(void);

            U32 m_cycles; //!< cycles executed
            U32 m_maxTime; //!< maximum execution time in microseconds
            volatile bool m_cycleStarted; //!< indicate that cycle has started. Used to detect overruns.
            NATIVE_UINT_TYPE m_contexts[NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS]; //!< Must match number of output ports
            NATIVE_INT_TYPE m_overrunThrottle; //!< throttle value for overrun events
            U32 m_cycleSlips; //!< tracks number of cycle slips
    };

}

#endif
