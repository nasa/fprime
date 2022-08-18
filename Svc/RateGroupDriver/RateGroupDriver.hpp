/**
 * \file
 * \author T. Canham
 * \brief RateGroupDivider component implementation
 *
 * This component implements a divider function. A primary tick is invoked
 * via the CycleIn port. The divider array then divides down the tick into
 * CycleOut ports. The ports are called at the rate of
 * input rate/divider[port]
 *
 * \copyright
 * Copyright 2009-2015, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 * <br /><br />
 */

#ifndef SVC_RATEGROUPDRIVER_HPP
#define SVC_RATEGROUPDRIVER_HPP

#include <Svc/RateGroupDriver/RateGroupDriverComponentAc.hpp>
#include <FpConfig.hpp>

namespace Svc {

    //! \class RateGroupDriver
    //! \brief Implementation class for RateGroupDriver
    //!
    //! Takes the input from CycleIn and divides it.
    //! Output rate is CycleIn rate/divider[port]
    //!

    class RateGroupDriver : public RateGroupDriverComponentBase {

        public:

            //!  \brief RateGroupDriver constructor
            //!
            //!  The constructor takes the divider array and stores it
            //!  for use when the CycleIn port is called.
            //!
            //!  \param compName component name
            //!
            RateGroupDriver(const char* compName);

            //!  \brief RateGroupDriver initialization function
            //!
            //!  The init() function initializes the autocoded base class

            void init(NATIVE_INT_TYPE instanceId = 0);

            //!  \brief RateGroupDriver configuration function
            //!  \param dividers array of integers used to divide down input tick
            //!  \param numDividers size of dividers array

            void configure(NATIVE_INT_TYPE dividers[], NATIVE_INT_TYPE numDividers);

            //!  \brief RateGroupDriverImpl destructor

            ~RateGroupDriver();

        PRIVATE:

            //! downcall for input port
            //! NOTE: This port can execute in ISR context.
            void CycleIn_handler(NATIVE_INT_TYPE portNum, Svc::TimerVal& cycleStart);

            //! divider array
            NATIVE_INT_TYPE m_dividers[NUM_CYCLEOUT_OUTPUT_PORTS];

            //! size of divider array
            NATIVE_INT_TYPE m_numDividers;

            //! tick counter
            NATIVE_INT_TYPE m_ticks;
            //! rollover counter
            NATIVE_INT_TYPE m_rollover;
        public:
            //! Size of the divider table, provided as a constants to users passing the table in
            static const NATIVE_UINT_TYPE DIVIDER_SIZE = NUM_CYCLEOUT_OUTPUT_PORTS;
    };

}

#endif
