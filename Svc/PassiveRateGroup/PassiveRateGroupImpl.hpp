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

#ifndef SVC_PASSIVERATEGROUP_IMPL_HPP
#define SVC_PASSIVERATEGROUP_IMPL_HPP

#include <Svc/PassiveRateGroup/PassiveRateGroupComponentAc.hpp>

namespace Svc {

    //! \class PassiveRateGroupImpl
    //! \brief Executes a set of components as part of a rate group
    //!
    //! PassiveRateGroup takes an input cycle call to begin the rate group cycle.
    //! It calls each output port in succession and passes the value in the context
    //! array at the index corresponding to the output port number. It keeps track of the execution
    //! time of the rate group and detects overruns.
    //!

    class PassiveRateGroupImpl : public PassiveRateGroupComponentBase {
        public:

            //!  \brief PassiveRateGroupImpl constructor
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
            PassiveRateGroupImpl(const char* compName, NATIVE_UINT_TYPE contexts[], NATIVE_UINT_TYPE numContexts);
#else
            PassiveRateGroupImpl(NATIVE_UINT_TYPE contexts[], NATIVE_UINT_TYPE numContexts);
#endif
      
            //!  \brief PassiveRateGroupImpl initialization function
            //!
            //!  The initialization function of the class initializes the member
            //!  ports and the component base class
            //!
            //!  \param instance Identifies the instance of the rate group component

            void init(NATIVE_INT_TYPE instance);

            //!  \brief PassiveRateGroupImpl destructor
            //!
            //!  The destructor of the class is empty

            ~PassiveRateGroupImpl(void);

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

            U32 m_cycles; //!< cycles executed
            U32 m_maxTime; //!< maximum execution time in microseconds
            NATIVE_UINT_TYPE m_contexts[NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS]; //!< Must match number of output ports

        public:

            // put this constant here for sizing the context table
            static const NATIVE_UINT_TYPE CONTEXT_SIZE =  NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS;

    };

}

#endif
