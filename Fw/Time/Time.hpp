#ifndef FW_TIME_HPP
#define FW_TIME_HPP

#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/Serializable.hpp>
#include <FpConfig.hpp>

namespace Fw {
    class Time: public Serializable {
        public:

            enum {
                SERIALIZED_SIZE = sizeof(FwTimeBaseStoreType)
                    + sizeof(FwTimeContextStoreType)
                    + sizeof(U32) + sizeof(U32)
            };

            Time(void); // !< Default constructor
            Time(const Time& other); // !< Copy constructor
            Time(U32 seconds, U32 useconds); // !< Constructor with member values as arguments
            Time(TimeBase timeBase, U32 seconds, U32 useconds); // !< Constructor with member values as arguments
            Time(TimeBase timeBase, FwTimeContextStoreType context, U32 seconds, U32 useconds); // !< Constructor with member values as arguments
            virtual ~Time(void); // !< Destructor
            void set(U32 seconds, U32 useconds); // !< Sets value of time stored
            void set(TimeBase timeBase, U32 seconds, U32 useconds); // !< Sets value of time stored
            void set(TimeBase timeBase, FwTimeContextStoreType context, U32 seconds, U32 useconds); // !< Sets value of time stored
            void setTimeBase(TimeBase timeBase);
            void setTimeContext(FwTimeContextStoreType context);
            U32 getSeconds(void) const; // !< Gets seconds part of time
            U32 getUSeconds(void) const; // !< Gets microseconds part of time
            TimeBase getTimeBase(void) const; // !< Time base of time. This is project specific and is meant for indicating different sources of time
            FwTimeContextStoreType getContext(void) const; // !< get the context value
            SerializeStatus serialize(SerializeBufferBase& buffer) const; // !< Serialize method
            SerializeStatus deserialize(SerializeBufferBase& buffer); // !< Deserialize method
            bool operator==(const Time& other) const;
            bool operator!=(const Time& other) const;
            bool operator>(const Time& other) const;
            bool operator<(const Time& other) const;
            bool operator>=(const Time& other) const;
            bool operator<=(const Time& other) const;
            const Time& operator=(const Time& other);

            // Static methods:
            //! The type of a comparison result
            typedef enum {
              LT = -1,
              EQ = 0,
              GT = 1
            } Comparison;

            //! \return time zero
            static Time zero(TimeBase timeBase=TB_NONE);

            //! Compare two times
            //! \return The result
            static Comparison compare(
                const Time &time1, //!< Time 1
                const Time &time2 //!< Time 2
            );

            //! Add two times
            //! \return The result
            static Time add(
                const Time& a, //!< Time a
                const Time& b //!< Time b
            );

            //! Subtract subtrahend from minuend
            //! \return The result
            static Time sub(
                const Time& minuend, //!< Value being subtracted from
                const Time& subtrahend //!< Value being subtracted
            );

            // add seconds and microseconds to existing time
            void add(U32 seconds, U32 mseconds);

#ifdef BUILD_UT // Stream operators to support Googletest
            friend std::ostream& operator<<(std::ostream& os,  const Time& val);
#endif
        PRIVATE:
            U32 m_seconds; // !< seconds portion
            U32 m_useconds; // !< microseconds portion
            TimeBase m_timeBase; // !< basis of time (defined by system)
            FwTimeContextStoreType m_timeContext; // !< user settable value. Could be reboot count, node, etc
    };
    const static Time ZERO_TIME = Time();

}

#endif
