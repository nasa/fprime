// ======================================================================
// \title  EmptyTest.cpp
// \author R. Bocchino
// \brief  cpp file for EmptyTest class
// ======================================================================

#include "FppTest/struct/EmptySerializableAc.hpp"
#include "FppTest/utils/Utils.hpp"

#include "Fw/Types/SerialBuffer.hpp"
#include "Fw/Types/StringTemplate.hpp"
#include "Fw/Types/StringUtils.hpp"
#include "STest/Pick/Pick.hpp"

#include "gtest/gtest.h"

#include <sstream>

namespace FppTest {

namespace Struct {

// Test struct constants and default constructor
TEST(EmptyTest, Default) {
    Empty e;
    ASSERT_EQ(Empty::SERIALIZED_SIZE, 0);
    ASSERT_EQ(e.serializedSize(), 0);
}

// Test struct constructors
TEST(EmptyTest, Constructors) {
    // Copy constructor
    Empty e1;
    Empty e2(e1);
}

// Test struct assignment operator
TEST(EmptyTest, AssignmentOp) {
    Empty e1;
    Empty e2;
    // Copy assignment
    e1 = e2;
}

// Test struct equality and inequality operators
TEST(EmptyTest, EqualityOp) {
    Empty e1;
    Empty e2;
    ASSERT_TRUE(e1 == e2);
    ASSERT_FALSE(e1 != e2);
}

// Test struct serialization and deserialization
TEST(EmptyTest, Serialization) {
    Empty e1;
    Empty e2;

    constexpr FwSizeType sizeOfData = 1;
    U8 data[sizeOfData];
    Fw::ExternalSerializeBuffer buf(data, sizeOfData);

    // Serialize
    {
        const auto status = buf.serializeFrom(e1);
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(buf.getSize(), 0);
    }

    // Deserialize
    {
        const auto status = buf.deserializeTo(e2);
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(e1, e2);
    }
}

// Test struct toString() and ostream operator functions
TEST(EmptyTest, ToString) {
    Empty e;
    std::stringstream buf1;
    std::stringstream buf2;

    buf1 << e;
    buf2 << "()";

    // Truncate string output
    Fw::String s2(buf2.str().c_str());

    ASSERT_STREQ(buf1.str().c_str(), s2.toChar());
}

}  // namespace Struct

}  // namespace FppTest
