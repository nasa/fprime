// ======================================================================
// \title  SerializeBufferBaseTester.hpp
// \author m-aleem
// \brief  hpp file for SerializeBufferBaseTester
// ======================================================================

#ifndef FW_SerializeBufferBaseTester_HPP
#define FW_SerializeBufferBaseTester_HPP

#include <gtest/gtest.h>
#include <Fw/Types/Serializable.hpp>

namespace Fw {

class SerializeBufferBaseTester {
  public:
    // Assertion methods for serialization location
    static void assertSerLoc(const Fw::SerializeBufferBase& buff, FwSizeType expected) {
        ASSERT_EQ(expected, buff.m_serLoc);
    }

    static void assertDeserLoc(const Fw::SerializeBufferBase& buff, FwSizeType expected) {
        ASSERT_EQ(expected, buff.m_deserLoc);
    }

    // Reset verification
    static void assertResetState(const Fw::SerializeBufferBase& buff) {
        ASSERT_EQ(0, buff.m_serLoc);
        ASSERT_EQ(0, buff.m_deserLoc);
    }

    // Verify serialization of different data types
    static void verifyU8Serialization(Fw::SerializeBufferBase& buff, U8 value) {
        // Save the current serialization location
        FwSizeType prevSerLoc = buff.m_serLoc;

        // Serialize the value
        Fw::SerializeStatus status = buff.serialize(value);

        // Verify serialization was successful and pointer advanced correctly
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, status);
        ASSERT_EQ(prevSerLoc + sizeof(U8), buff.m_serLoc);
    }

    static void verifyI8Serialization(Fw::SerializeBufferBase& buff, I8 value) {
        // Save the current serialization location
        FwSizeType prevSerLoc = buff.m_serLoc;

        // Serialize the value
        Fw::SerializeStatus status = buff.serialize(value);

        // Verify serialization was successful and pointer advanced correctly
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, status);
        ASSERT_EQ(prevSerLoc + sizeof(I8), buff.m_serLoc);
    }

    // Verify deserialization of different data types
    static void verifyU8Deserialization(Fw::SerializeBufferBase& buff, U8& actualValue, U8 expectedValue) {
        FwSizeType prevDeserLoc = buff.m_deserLoc;

        Fw::SerializeStatus status = buff.deserialize(actualValue);

        ASSERT_EQ(Fw::FW_SERIALIZE_OK, status);
        ASSERT_EQ(expectedValue, actualValue);
        ASSERT_EQ(prevDeserLoc + sizeof(U8), buff.m_deserLoc);
    }

    static void verifyI8Deserialization(Fw::SerializeBufferBase& buff, I8& actualValue, I8 expectedValue) {
        FwSizeType prevDeserLoc = buff.m_deserLoc;

        Fw::SerializeStatus status = buff.deserialize(actualValue);

        ASSERT_EQ(Fw::FW_SERIALIZE_OK, status);
        ASSERT_EQ(expectedValue, actualValue);
        ASSERT_EQ(prevDeserLoc + sizeof(I8), buff.m_deserLoc);
    }

    // Verification methods for comparing serialization location
    static void verifySerLocLT(const Fw::SerializeBufferBase& buff, FwSizeType maxValue) {
        ASSERT_LT(buff.m_serLoc, maxValue);
    }

    static void verifySerLocEq(const Fw::SerializeBufferBase& buff, FwSizeType expected) {
        ASSERT_EQ(expected, buff.m_serLoc);
    }

    static void verifyDeserLocEq(const Fw::SerializeBufferBase& buff, FwSizeType expected) {
        ASSERT_EQ(expected, buff.m_deserLoc);
    }
};

}  // namespace Fw

#endif  // FW_SerializeBufferBaseTester_HPP
