#include <Autocoders/Python/test/enum_xml/Component1ComponentAc.hpp>
#include <Autocoders/Python/test/enum_xml/Component1Impl.hpp>
#include <Autocoders/Python/test/enum_xml/Enum1EnumAc.hpp>
#include <Autocoders/Python/test/enum_xml/Port1PortAc.hpp>
#include <Autocoders/Python/test/enum_xml/Serial1SerializableAc.hpp>

#include <Fw/Types/SerialBuffer.hpp>
#include <Fw/Types/Assert.hpp>

#include "Fw/Test/UnitTestAssert.hpp"
#include "STest/Pick/Pick.hpp"
#include "STest/Random/Random.hpp"
#include "gtest/gtest.h"

#include <iostream>
#include <cstring>

using namespace std;

// Instantiate the inst1 and inst2 components
Example::ExampleEnumImpl inst1("inst1");
Example::ExampleEnumImpl inst2("inst2");

void constructArchitecture() {

    // Connect inst1 to inst2
    inst1.set_EnumOut_OutputPort(0, inst2.get_EnumIn_InputPort(0));

    // Connect inst2 to inst1
    inst2.set_EnumOut_OutputPort(0, inst1.get_EnumIn_InputPort(0));

    // Instantiate components
    inst1.init(100);
    inst2.init(100);

}

Example::Enum1::t getEnumConstant() {
    Example::Enum1::t c = Example::Enum1::Item1;
    const U32 i = STest::Pick::lowerUpper(0, 4);

    switch(i) {
        case 0:
            c = Example::Enum1::Item1;
            break;
        case 1:
            c = Example::Enum1::Item2;
            break;
        case 2:
            c = Example::Enum1::Item3;
            break;
        case 3:
            c = Example::Enum1::Item4;
            break;
        case 4:
            c = Example::Enum1::Item5;
            break;
        default:
            FW_ASSERT(0, i);
            break;
    }

    return c;
}

Example::Enum1::t getNonnegativeConstant() {
    Example::Enum1::t c = Example::Enum1::Item1;
    const U32 i = STest::Pick::lowerUpper(0, 2);

    switch(i) {
        case 0:
            c = Example::Enum1::Item2;
            break;
        case 1:
            c = Example::Enum1::Item3;
            break;
        case 2:
            c = Example::Enum1::Item4;
            break;
        default:
            FW_ASSERT(0, i);
            break;
    }

    return c;
}

Example::Enum1::t getNegativeConstant() {
    Example::Enum1::t c = Example::Enum1::Item1;
    const U32 i = STest::Pick::lowerUpper(0, 1);

    switch(i) {
        case 0:
            c = Example::Enum1::Item1;
            break;
        case 1:
            c = Example::Enum1::Item5;
            break;
        default:
            FW_ASSERT(0, i);
            break;
    }

    return c;
}

Example::Enum1 getEnum() {
    const Example::Enum1 e = getEnumConstant();
    return e;
}

Example::Enum1 getEnumFromI32() {
    Example::Enum1 e = getEnumConstant();
    e = static_cast<I32>(getEnumConstant());
    return e;
}

Example::Enum1 getEnumFromU32() {
    Example::Enum1 e = getNonnegativeConstant();
    e = static_cast<U32>(getNonnegativeConstant());
    return e;
}

void checkAssertionFailure(
    const ::Test::UnitTestAssert& uta,
    const U32 expectedLineNumber,
    const U32 expectedArg1
) {
  ASSERT_TRUE(uta.assertFailed());
  Test::UnitTestAssert::File file = Test::UnitTestAssert::fileInit;
  NATIVE_UINT_TYPE lineNo = 0;
  NATIVE_UINT_TYPE numArgs = 0;
  FwAssertArgType arg1 = 0;
  FwAssertArgType arg2 = 0;
  FwAssertArgType arg3 = 0;
  FwAssertArgType arg4 = 0;
  FwAssertArgType arg5 = 0;
  FwAssertArgType arg6 = 0;
  uta.retrieveAssert(file, lineNo, numArgs, arg1, arg2, arg3, arg4, arg5, arg6);
  ASSERT_EQ(expectedLineNumber, lineNo);
  ASSERT_EQ(1U, numArgs);
  ASSERT_EQ(expectedArg1, arg1);
}

TEST(EnumXML, InvalidNegativeConstant) {
  ::Test::UnitTestAssert uta;
  Example::Enum1 enum1 = getEnum();
  // Get a valid negative constant
  const I32 negativeConstant = getNegativeConstant();
  const U32 expectedLineNumber = 61;
  // Turn it into a U32
  const U32 expectedArg1 = negativeConstant;
  // As a U32, the constant is not valid
  // This should cause an assertion failure
  enum1 = expectedArg1;
  checkAssertionFailure(uta, expectedLineNumber, expectedArg1);
}

TEST(EnumXML, InvalidConstant) {
  ::Test::UnitTestAssert uta;
  Example::Enum1 enum1 = getEnum();
  // Get an invalid constant
  const I32 invalidConstant = 42;
  // This should cause an assertion failure
  enum1 = invalidConstant;
  const U32 expectedLineNumber = 54;
  const U32 expectedArg1 = invalidConstant;
  checkAssertionFailure(uta, expectedLineNumber, expectedArg1);
}

TEST(EnumXML, OK) {

    // Explicitly set enum1 to the default value
    Example::Enum1 enum1(Example::Enum1::Item4);
    Example::Enum1 enum2;
    Example::Enum1 enum3;
    Example::Enum2 enum4;
    Example::Enum3 enum5;

    Example::Serial1 serial1;

    // Check that other enums are set to default value
    ASSERT_EQ(enum1, enum2);
    ASSERT_EQ(enum1, enum3);

    // Check that enum are set to uninitialized value
    ASSERT_EQ(enum4.e, 0);

    // Check that the enum serializable types are set correctly
    ASSERT_EQ(Example::Enum1::SERIALIZED_SIZE, sizeof(FwEnumStoreType));
    ASSERT_EQ(Example::Enum2::SERIALIZED_SIZE, sizeof(U64));
    ASSERT_EQ(Example::Enum3::SERIALIZED_SIZE, sizeof(U8));

    enum1 = getEnumFromI32();
    cout << "Created first enum: " << enum1 << endl;

    enum2 = getEnum();
    cout << "Created second enum: " << enum2 << endl;

    enum3 = getEnumFromU32();
    cout << "Created third enum: " << enum3 << endl;

    // Save copy of enums to test against post-serialization
    Example::Enum1 enum1Save = enum1;
    Example::Enum1 enum2Save = enum2;

    int serial_arg1 = 0;

    int serial_arg2 = 0;

    // Serialize enums
    U8 buffer1[1024];
    U8 buffer2[1024];
    U8 buffer3[1024];
    U8 buffer4[1024];
    U8 buffer5[1024];
    Fw::SerialBuffer enumSerial1 = Fw::SerialBuffer(buffer1, sizeof(buffer1));
    Fw::SerialBuffer enumSerial2 = Fw::SerialBuffer(buffer2, sizeof(buffer2));
    Fw::SerialBuffer enumSerial3 = Fw::SerialBuffer(buffer3, sizeof(buffer3));
    Fw::SerialBuffer enumSerial4 = Fw::SerialBuffer(buffer4, sizeof(buffer4));
    Fw::SerialBuffer enumSerial5 = Fw::SerialBuffer(buffer5, sizeof(buffer5));
    ASSERT_EQ(enumSerial1.serialize(enum1), Fw::FW_SERIALIZE_OK);
    cout << "Serialized enum1" << endl;

    ASSERT_EQ(enumSerial2.serialize(enum2), Fw::FW_SERIALIZE_OK);
    cout << "Serialized enum2" << endl;

    ASSERT_EQ(enumSerial3.serialize(enum3), Fw::FW_SERIALIZE_OK);
    cout << "Serialized enum3" << endl;

    ASSERT_EQ(enumSerial4.serialize(enum4), Fw::FW_SERIALIZE_OK);
    cout << "Serialized enum4" << endl;

    ASSERT_EQ(enumSerial5.serialize(enum5), Fw::FW_SERIALIZE_OK);
    cout << "Serialized enum5" << endl;

    cout << "Serialized enums" << endl;

    // Check that the serialized types are correctly set
    ASSERT_EQ(enumSerial1.getBuffLength(), sizeof(FwEnumStoreType));
    ASSERT_EQ(enumSerial2.getBuffLength(), sizeof(FwEnumStoreType));
    ASSERT_EQ(enumSerial3.getBuffLength(), sizeof(FwEnumStoreType));
    ASSERT_EQ(enumSerial4.getBuffLength(), sizeof(U64));
    ASSERT_EQ(enumSerial5.getBuffLength(), sizeof(U8));

    // Deserialize enums
    ASSERT_EQ(enumSerial1.deserialize(enum1Save), Fw::FW_SERIALIZE_OK);
    cout << "Deserialized enum1" << endl;

    ASSERT_EQ(enumSerial2.deserialize(enum2Save), Fw::FW_SERIALIZE_OK);
    cout << "Deserialized enum2" << endl;

    ASSERT_EQ(enum1, enum1Save);
    ASSERT_FALSE(enum1 != enum1Save);
    cout << "Successful enum1 check" << endl;

    ASSERT_EQ(enum2, enum2Save);
    ASSERT_FALSE(enum2 != enum2Save);
    cout << "Successful enum2 check" << endl;

    cout << "Deserialized enums" << endl;

    // Create serializable and test that enum is saved
    serial1 = Example::Serial1(serial_arg1, serial_arg2, enum2);
    ASSERT_EQ(serial1.getMember3(), enum2);
    cout << "Created serializable with enum arg" << endl;

    cout << "Created serializable" << endl;

    // Invoke ports to test enum usage
    cout << "Invoking inst1..." << endl;
    inst1.get_ExEnumIn_InputPort(0)->invoke(enum1, serial1);
    inst1.doDispatch();
    inst1.get_ExEnumIn_InputPort(0)->invoke(enum2, serial1);
    inst1.doDispatch();

    cout << "Invoking inst2..." << endl;
    inst2.get_ExEnumIn_InputPort(0)->invoke(enum1, serial1);
    inst2.doDispatch();
    inst2.get_ExEnumIn_InputPort(0)->invoke(enum2, serial1);
    inst2.doDispatch();

    cout << "Invoked ports" << endl;
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();

    constructArchitecture();

    int status = RUN_ALL_TESTS();

    return status;
}
