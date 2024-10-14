#ifndef FW_TIME_INTERVAL_HPP
#define FW_TIME_INTERVAL_HPP

#include <FpConfig.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/Serializable.hpp>

//!
//! @class TimeInterval
//! @brief A class to represent a time interval holding two U32 seconds and microseconds values.
//! 
//! The TimeInterval class is designed to hold a time interval and provides various methods 
//! to manipulate and compare time intervals. It supports serialization and deserialization 
//! for easy storage and transmission.
//!
namespace Fw {

    class TimeInterval: public Serializable {
        public:

            enum {
                SERIALIZED_SIZE = sizeof(U32) * 2
            };

            TimeInterval() = default; // !< Default constructor
            ~TimeInterval() = default; // !< Default destructor
            TimeInterval(const TimeInterval& other); // !< Copy constructor
            TimeInterval(U32 seconds, U32 useconds); // !< Constructor with member values as arguments
            void set(U32 seconds, U32 useconds); // !< Sets value of time stored
            U32 getSeconds() const; // !< Gets seconds part of time
            U32 getUSeconds() const; // !< Gets microseconds part of time
            SerializeStatus serialize(SerializeBufferBase& buffer) const; // !< Serialize method
            SerializeStatus deserialize(SerializeBufferBase& buffer); // !< Deserialize method
            void add(U32 seconds, U32 mseconds); // !< Add seconds and microseconds to existing time interval
            bool operator==(const TimeInterval& other) const;
            bool operator!=(const TimeInterval& other) const;
            bool operator>(const TimeInterval& other) const;
            bool operator<(const TimeInterval& other) const;
            bool operator>=(const TimeInterval& other) const;
            bool operator<=(const TimeInterval& other) const;
            TimeInterval& operator=(const TimeInterval& other);

            //! The type of a comparison result
            typedef enum {
              LT = -1,
              EQ = 0,
              GT = 1,
              INCOMPARABLE = 2
            } Comparison;

            //! Compare two time intervals
            //! A time interval is considered greater than another if it spans a longer duration
            //! The comparison is done on the seconds first, then the microseconds if the seconds are equal
            //! \return TimeInterval result
            static Comparison compare(
                const TimeInterval &time1, //!< TimeInterval 1
                const TimeInterval &time2 //!< TimeInterval 2
            );

            //! Add two time intervals
            //! Adds the seconds and microseconds fields of two time intervals together
            //! \return TimeInterval result
            static TimeInterval add(
                const TimeInterval& a, //!< TimeInterval a
                const TimeInterval& b //!< TimeInterval b
            );

            //! Subtract two time intervals
            //! This computes the absolute value of the difference between two time intervals
            //! For example if t1=(0s, 5us) and t2=(0s, 3us), the result is (0s, 2us).
            //! This operation is commutative, i.e. the result is the same regardless of the order of the arguments.
            //! \return TimeInterval result
            static TimeInterval sub(
                const TimeInterval& t1, //!< TimeInterval 1
                const TimeInterval& t2 //!< TimeInterval 2
            );



#ifdef BUILD_UT // Stream operators to support Googletest
            friend std::ostream& operator<<(std::ostream& os,  const TimeInterval& val);
#endif
        PRIVATE:
            U32 m_seconds; // !< seconds portion of TimeInterval
            U32 m_useconds; // !< microseconds portion of TimeInterval
    };

}

#endif
