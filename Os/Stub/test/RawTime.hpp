// ======================================================================
// \title Os/Stub/RawTime.hpp
// \brief stub definitions for Os::RawTime
// ======================================================================
#include "Os/RawTime.hpp"

#ifndef OS_STUB_RAWTIME_HPP
#define OS_STUB_RAWTIME_HPP
namespace Os {
namespace Stub {
namespace RawTime {
namespace Test {

//! Data that supports the stubbed Mutex implementation.
//!/
struct StaticData {
    //! Enumeration of last function called
    //!
    enum LastFn {
        NONE_FN,
        GET_TIME_FN,
        GET_DIFF_FN,
        GET_INTERVAL_FN,
        SERIALIZE_FN,
        DESERIALIZE_FN,
        GET_HANDLE_FN,
        CONSTRUCT_FN,
        DESTRUCT_FN
    };
    StaticData() = default;
    ~StaticData() = default;

    //! Last function called
    LastFn lastCalled = NONE_FN;

    Os::RawTime::Status lastStatus = Os::RawTime::Status::OP_OK;

    // Singleton data
    static StaticData data;
};
struct TestRawTimeHandle : public RawTimeHandle {};

//! \brief Test implementation of Os::RawTime
//!
//! Test implementation of `RawTimeInterface`.
//!
class TestRawTime : public RawTimeInterface {
  public:
    //! \brief constructor
    //!
    TestRawTime();

    //! \brief destructor
    //!
    ~TestRawTime() override;

    //! \brief return the underlying RawTime handle (implementation specific)
    //! \return internal RawTime handle representation
    RawTimeHandle* getHandle() override;

    Status getRawTime() override;
    Status getDiffUsec(const RawTimeHandle& other, U32& result) const override;
    Status getTimeInterval(const RawTimeHandle& other, Fw::TimeInterval& interval) const override;  //!<  docs
    Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) const override;  //!< serialize contents
    Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer) override;      //!< deserialize to contents

  private:
    //! Handle for TestRawTime
    TestRawTimeHandle m_handle;
};

} // namespace Test
} // namespace RawTime
} // namespace Test
} // namespace Os
#endif // OS_Test_RAWTIME_HPP
