// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "FppTest/dpTool/test/ut/Tester.hpp"
#include "Fw/Test/UnitTest.hpp"
#include "STest/Random/Random.hpp"

using namespace FppTest;

TEST(makeU32, OK) {
    Tester tester;
    tester.makeU32();
}

TEST(makeComplex, OK) {
    Tester tester;
    tester.makeComplex();
}
TEST(makeU8Array, OK) {
    Tester tester;
    tester.makeU8Array();
}

TEST(makeU32Array, OK) {
    Tester tester;
    tester.makeU32Array();
}

TEST(makeDataArray, OK) {
    Tester tester;
    tester.makeDataArray();
}

TEST(makeEnum, OK) {
    Tester tester;
    tester.makeEnum();
}

TEST(makeF32, OK) {
    Tester tester;
    tester.makeF32();
}

TEST(makeF64, OK) {
    Tester tester;
    tester.makeF64();
}

TEST(makeBool, OK) {
    Tester tester;
    tester.makeBool();
}

TEST(makeI8, OK) {
    Tester tester;
    tester.makeI8();
}

TEST(makeI16, OK) {
    Tester tester;
    tester.makeI16();
}

TEST(makeI32, OK) {
    Tester tester;
    tester.makeI32();
}

TEST(makeI64, OK) {
    Tester tester;
    tester.makeI64();
}

TEST(makeFppArray, OK) {
    Tester tester;
    tester.makeFppArray();
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
