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
            //! Size of the divider table, provided as a constants to users passing the table in
            static const NATIVE_UINT_TYPE DIVIDER_SIZE = NUM_CYCLEOUT_OUTPUT_PORTS;

            //! \class Divider
            //! \brief Struct describing a divider
            struct Divider{
                //! Initializes divisor and offset to 0 (unused)
                Divider() : divisor(0), offset(0)
                {}
                //! Initializes divisor and offset to passed-in pair 
                Divider(NATIVE_INT_TYPE divisorIn, NATIVE_INT_TYPE offsetIn) :
                    divisor(divisorIn), offset(offsetIn)
                {}
                //! Divisor
                NATIVE_INT_TYPE divisor;
                //! Offset
                NATIVE_INT_TYPE offset;
            };

            //! \class DividerSet
            //! \brief Struct containing an array of dividers
            struct DividerSet {
                //! Dividers
                Divider dividers[Svc::RateGroupDriver::DIVIDER_SIZE];
            };

            //!  \brief RateGroupDriver constructor
            //!
            //!  The constructor takes the divider array and stores it
            //!  for use when the CycleIn port is called.
            //!
            //!  \param compName component name
            //!
            RateGroupDriver(const char* compName);

            //!  \brief RateGroupDriver configuration function
            //!  \param dividersSet set of dividers used to divide down input tick

            void configure(const DividerSet& dividersSet);

            //!  \brief RateGroupDriverImpl destructor

            ~RateGroupDriver();

        PRIVATE:

            //! downcall for input port
            //! NOTE: This port can execute in ISR context.
            void CycleIn_handler(NATIVE_INT_TYPE portNum, Svc::TimerVal& cycleStart);

            //! divider array
            Divider m_dividers[NUM_CYCLEOUT_OUTPUT_PORTS];

            //! tick counter
            NATIVE_INT_TYPE m_ticks;

            //! rollover counter
            NATIVE_INT_TYPE m_rollover;

            //! has the configure method been called
            bool m_configured;
    };

}

#endif
