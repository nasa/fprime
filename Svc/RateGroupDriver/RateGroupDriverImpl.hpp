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

#ifndef SVC_RATEGROUPDRIVER_IMPL_HPP
#define SVC_RATEGROUPDRIVER_IMPL_HPP

#include <Svc/RateGroupDriver/RateGroupDriverComponentAc.hpp>
#include <Fw/Types/BasicTypes.hpp>

namespace Svc {

    //! \class RateGroupDriverImpl
    //! \brief Implementation class for RateGroupDriver
    //!
    //! Takes the input from CycleIn and divides it.
    //! Output rate is CycleIn rate/divider[port]
    //!

    class RateGroupDriverImpl : public RateGroupDriverComponentBase {

        public:

            //!  \brief RateGroupDriverImpl constructor
            //!
            //!  The constructor takes the divider array and stores it
            //!  for use when the CycleIn port is called.
            //!
            //!  \param compName component name
            //!  \param dividers array of integers used to divide down input tick
            //!  \param numDividers size of dividers array
            //!
            //!  \return return value description
#if FW_OBJECT_NAMES == 1        
            RateGroupDriverImpl(const char* compName, NATIVE_INT_TYPE dividers[], NATIVE_INT_TYPE numDividers);
#else
            RateGroupDriverImpl(NATIVE_INT_TYPE dividers[], NATIVE_INT_TYPE numDividers);
#endif

            //!  \brief RateGroupDriverImpl initialization function
            //!
            //!  The init() function initializes the autocoded base class

            void init(void);

            //!  \brief RateGroupDriverImpl destructor

            ~RateGroupDriverImpl(void);

        PRIVATE:

            //! downcall for input port
            //! NOTE: This port can execute in ISR context.
            void CycleIn_handler(NATIVE_INT_TYPE portNum, Svc::TimerVal& cycleStart);

            //! divider array
            NATIVE_INT_TYPE m_dividers[NUM_CYCLEOUT_OUTPUT_PORTS];

            //! tick counter
            NATIVE_INT_TYPE m_ticks;
            //! rollover counter
            NATIVE_INT_TYPE m_rollover;
    };

}

#endif
