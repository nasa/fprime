// ======================================================================
// \title  LinearBufferBaseTester.hpp
// \author m-aleem
// \brief  hpp file for LinearBufferBaseTester
// ======================================================================

#ifndef FW_LinearBufferBaseTester_HPP
#define FW_LinearBufferBaseTester_HPP

#include <gtest/gtest.h>
#include <Fw/Types/Serializable.hpp>

namespace Fw {

class LinearBufferBaseTester {
  public:
    // Assertion methods for serialization location
    static void assertSerLoc(const Fw::LinearBufferBase& buff, FwSizeType expected) {
        ASSERT_EQ(expected, buff.m_serLoc);
    }

    static void assertDeserLoc(const Fw::LinearBufferBase& buff, FwSizeType expected) {
        ASSERT_EQ(expected, buff.m_deserLoc);
    }

    // Reset verification
    static void assertResetState(const Fw::LinearBufferBase& buff) {
        ASSERT_EQ(0, buff.m_serLoc);
        ASSERT_EQ(0, buff.m_deserLoc);
    }

    // Verify serialization of different data types
    static void verifyU8Serialization(Fw::LinearBufferBase& buff, U8 value) {
        // Save the current serialization location
        FwSizeType prevSerLoc = buff.m_serLoc;

        // Serialize the value
        Fw::SerializeStatus status = buff.serializeFrom(value);

        // Verify serialization was successful and pointer advanced correctly
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, status);
        ASSERT_EQ(prevSerLoc + sizeof(U8), buff.m_serLoc);
    }

    static void verifyI8Serialization(Fw::LinearBufferBase& buff, I8 value) {
        // Save the current serialization location
        FwSizeType prevSerLoc = buff.m_serLoc;

        // Serialize the value
        Fw::SerializeStatus status = buff.serializeFrom(value);

        // Verify serialization was successful and pointer advanced correctly
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, status);
        ASSERT_EQ(prevSerLoc + sizeof(I8), buff.m_serLoc);
    }

    // Verify deserialization of different data types
    static void verifyU8Deserialization(Fw::LinearBufferBase& buff, U8& actualValue, U8 expectedValue) {
        FwSizeType prevDeserLoc = buff.m_deserLoc;

        Fw::SerializeStatus status = buff.deserializeTo(actualValue);

        ASSERT_EQ(Fw::FW_SERIALIZE_OK, status);
        ASSERT_EQ(expectedValue, actualValue);
        ASSERT_EQ(prevDeserLoc + sizeof(U8), buff.m_deserLoc);
    }

    static void verifyI8Deserialization(Fw::LinearBufferBase& buff, I8& actualValue, I8 expectedValue) {
        FwSizeType prevDeserLoc = buff.m_deserLoc;

        Fw::SerializeStatus status = buff.deserializeTo(actualValue);

        ASSERT_EQ(Fw::FW_SERIALIZE_OK, status);
        ASSERT_EQ(expectedValue, actualValue);
        ASSERT_EQ(prevDeserLoc + sizeof(I8), buff.m_deserLoc);
    }

    // Verification methods for comparing serialization location
    static void verifySerLocLT(const Fw::LinearBufferBase& buff, FwSizeType maxValue) {
        ASSERT_LT(buff.m_serLoc, maxValue);
    }

    static void verifySerLocEq(const Fw::LinearBufferBase& buff, FwSizeType expected) {
        ASSERT_EQ(expected, buff.m_serLoc);
    }

    static void verifyDeserLocEq(const Fw::LinearBufferBase& buff, FwSizeType expected) {
        ASSERT_EQ(expected, buff.m_deserLoc);
    }
};

}  // namespace Fw

#endif  // FW_LinearBufferBaseTester_HPP
