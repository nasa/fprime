/*
 * TimerVal.hpp
 *
 *  Created on: Aug 5, 2015
 *      Author: timothycanham
 */

#ifndef TIMERVAL_HPP_
#define TIMERVAL_HPP_

#include <Fw/Types/Serializable.hpp>
#include <Os/IntervalTimer.hpp>

namespace Svc {

    //! \class TimerVal
    //! \brief Serializable class for carrying timer values
    //!
    //! This class carries a timer value. It is meant to be
    //! passed through ports that carry timing info.

    class TimerVal: public Fw::Serializable {
        public:

            enum {
                SERIALIZED_SIZE = sizeof(U32) + sizeof(U32) //!< size of TimerVal private members
            };

            TimerVal(); //!< Default constructor

            //!  \brief Timer copy constructor
            //!
            //!  constructs a TimerVal with another timer as the source
            //!
            //!  \param other source timer value

            TimerVal(const TimerVal& other); //!< copy constructor

            //!  \brief Timer equal operator
            //!
            //!  copies timer value from another timer as the source
            //!
            //!  \param other source timer value

            TimerVal& operator=(const TimerVal& other); //!< equal operator

            //!  \brief Destructor
            //!
            //!  Does nothing
            //!

            virtual ~TimerVal() {}

            //!  \brief Serialization function
            //!
            //!  Serializes timer values
            //!
            //!  \param buffer destination buffer for serialization data

            Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) const; //!< serialize contents

            //!  \brief Deserialization function
            //!
            //!  Deserializes timer values
            //!
            //!  \param buffer source buffer for serialization data

            Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer); //!< deserialize to contents

            //!  \brief Returns the current timer value
		    //!
		    //!
            Os::IntervalTimer::RawTime getTimerVal() const;

            //!  \brief Function to store a timer value
            //!
            //!  This function calls the Os::IntervalTimer function to store a timer value in the private data of the object
            //!

            void take(); //!< Take raw time

            //!  \brief Compute difference function
            //!
            //!  This function computes the difference in time between the internal
            //!  value and the passed value. It is computed as internal - time parameter.
            //!
            //!  \param time time to compute difference from

            U32 diffUSec(const TimerVal& time); //!< takes difference between stored time and passed time

        PRIVATE:
            TimerVal(U32 upper, U32 lower); //!< Private constructor for testing
            Os::IntervalTimer::RawTime m_timerVal; //!< Stored timer value
    };

} /* namespace Svc */

#endif /* TIMERVAL_HPP_ */
