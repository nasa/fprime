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
 *   acknowledged.
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

class PassiveRateGroup : public PassiveRateGroupComponentBase {
  public:
    //!  \brief PassiveRateGroupImpl constructor
    //!
    //!  The constructor of the class clears all the flags and copies the
    //!  contents of the context array to private storage.
    //!
    //!  \param compName Name of the component
    explicit PassiveRateGroup(const char* compName);  //!  \brief PassiveRateGroupImpl initialization function

    //!  \brief PassiveRateGroup configuration function
    //!
    //!  The configuration function takes an array of context values to pass to
    //!  members of the rate group.
    //!
    //!  \param contexts Array of integers that contain the context values that will be sent
    //!         to each member component. The index of the array corresponds to the
    //!         output port number.
    //!  \param numContexts The number of elements in the context array.
    void configure(NATIVE_INT_TYPE contexts[], NATIVE_INT_TYPE numContexts);

    //!  \brief PassiveRateGroupImpl destructor
    //!
    //!  The destructor of the class is empty
    ~PassiveRateGroup();

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

    U32 m_cycles;                                                     //!< cycles executed
    U32 m_maxTime;                                                    //!< maximum execution time in microseconds
    NATIVE_INT_TYPE m_numContexts;                                    //!< number of contexts
    U32 m_contexts[NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS];              //!< Must match number of output ports
};

}  // namespace Svc

#endif
