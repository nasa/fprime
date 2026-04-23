#ifndef FW_TIME_HPP
#define FW_TIME_HPP

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Time/TimeComparisonEnumAc.hpp>
#include <Fw/Time/TimeValueSerializableAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/Serializable.hpp>
#include <config/TimeBaseEnumAc.hpp>

namespace Fw {
class Time : public Serializable {
    friend class TimeTester;

  public:
    enum { SERIALIZED_SIZE = sizeof(FwTimeBaseStoreType) + sizeof(FwTimeContextStoreType) + sizeof(U32) + sizeof(U32) };

    Time();                                              // !< Default constructor
    Time(const Time& other);                             // !< Copy constructor
    Time(U32 seconds, U32 useconds);                     // !< Constructor with member values as arguments
    Time(TimeBase timeBase, U32 seconds, U32 useconds);  // !< Constructor with member values as arguments
    Time(TimeBase timeBase,
         FwTimeContextStoreType context,
         U32 seconds,
         U32 useconds);  // !< Constructor with member values as arguments

    //! \brief Constructor with floating-point (F64) seconds
    //! \warning Negative value (seconds < 0) results in undefined behavior (assert fail in Debug)
    explicit Time(F64 seconds);

    virtual ~Time();                                         // !< Destructor
    void set(U32 seconds, U32 useconds);                     // !< Sets value of time stored
    void set(TimeBase timeBase, U32 seconds, U32 useconds);  // !< Sets value of time stored
    void set(TimeBase timeBase,
             FwTimeContextStoreType context,
             U32 seconds,
             U32 useconds);  // !< Sets value of time stored

    //! \brief Sets value of time stored
    //! \warning Negative value (seconds < 0) results in undefined behavior (assert fail in Debug)
    void set(F64 seconds);

    void setTimeBase(TimeBase timeBase);
    void setTimeContext(FwTimeContextStoreType context);
    U32 getSeconds() const;   // !< Gets seconds part of time
    U32 getUSeconds() const;  // !< Gets microseconds part of time
    TimeBase getTimeBase()
        const;  // !< Time base of time. This is project specific and is meant for indicating different sources of time
    FwTimeContextStoreType getContext() const;  // !< get the context value
    SerializeStatus serializeTo(SerialBufferBase& buffer,
                                Fw::Endianness mode = Fw::Endianness::BIG) const override;  // !< Serialize method
    SerializeStatus deserializeFrom(SerialBufferBase& buffer,
                                    Fw::Endianness mode = Fw::Endianness::BIG) override;  // !< Deserialize method
    bool operator==(const Time& other) const;
    bool operator!=(const Time& other) const;
    bool operator>(const Time& other) const;
    bool operator<(const Time& other) const;
    bool operator>=(const Time& other) const;
    bool operator<=(const Time& other) const;
    Time& operator=(const Time& other);

    //! \brief Assign this Time from floating-point (F64) seconds
    //! \warning Negative value (seconds < 0) results in undefined behavior (assert fail in Debug mode)
    Time& operator=(F64 seconds);

    //! \brief Add floating-point (F64) value to this Time
    //! \warning Negative value (seconds < 0) results in undefined behavior (assert fail in Debug mode)
    Time& operator+=(F64 seconds);

    //! \brief Convert object to floating point (F64) representation
    //! \return The floating point (F64) representation of this Time
    operator F64() const;

    //! \brief get the underlying TimeValue
    //! \return the TimeValue representation of this Time as a copy
    TimeValue asTimeValue() const;

    // Static methods:
    //! The type of a comparison result

    //! \return time zero
    static Time zero(TimeBase timeBase = TimeBase::TB_NONE);

    //! Compare two times
    //! \return The result
    static TimeComparison compare(const Time& time1,  //!< Time 1
                                  const Time& time2   //!< Time 2
    );

    //! Add two times
    //! \return The result
    static Time add(const Time& a,  //!< Time a
                    const Time& b   //!< Time b
    );

    //! Subtract subtrahend from minuend
    //! \return The result
    static Time sub(const Time& minuend,    //!< Value being subtracted from
                    const Time& subtrahend  //!< Value being subtracted
    );

    //! \brief Extract seconds from a floating-point (F64)
    //! \warning Negative value (seconds < 0) results in undefined behavior (assert fail in Debug mode)
    //! \return Seconds count as U32
    static U32 parseSeconds(F64 seconds);

    //! \brief Extract microseconds from a floating-point (F64)
    //! \warning Negative value (seconds < 0) results in undefined behavior (assert fail in Debug)
    //! \return Microseconds count as U32
    static U32 parseUSeconds(F64 seconds);

    // add seconds and microseconds to existing time
    void add(U32 seconds, U32 mseconds);

    //! \brief Add floating-point (F64) seconds to existing time
    //! \warning Negative value (seconds < 0) results in undefined behavior (assert fail in Debug)
    void add(F64 seconds);

#ifdef BUILD_UT  // Stream operators to support Googletest
    friend std::ostream& operator<<(std::ostream& os, const Time& val);
#endif
  private:
    TimeValue m_val;  // !< Time value
};
extern const Time ZERO_TIME;

}  // namespace Fw

#endif
