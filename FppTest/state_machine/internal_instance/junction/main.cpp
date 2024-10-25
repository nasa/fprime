// ----------------------------------------------------------------------
// main.cpp
// ----------------------------------------------------------------------

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/junction/Basic.hpp"
#include "FppTest/state_machine/internal_instance/junction/BasicU32.hpp"
#include "FppTest/state_machine/internal_instance/junction/InputPairU16U32.hpp"
#include "FppTest/state_machine/internal_instance/junction/JunctionToJunction.hpp"
#include "FppTest/state_machine/internal_instance/junction/JunctionToState.hpp"
#include "FppTest/state_machine/internal_instance/junction/Sequence.hpp"
#include "STest/STest/Random/Random.hpp"

TEST(Basic, BasicTrue) {
    FppTest::SmInstanceJunction::Basic basic("basic");
    basic.testBasicTrue();
}

TEST(Basic, BasicFalse) {
    FppTest::SmInstanceJunction::Basic basic("basic");
    basic.testBasicFalse();
}

TEST(Basic, SmJunctionBasicTrue) {
    FppTest::SmInstanceJunction::Basic basic("basic");
    basic.testSmJunctionBasicTrue();
}

TEST(Basic, SmJunctionBasicFalse) {
    FppTest::SmInstanceJunction::Basic basic("basic");
    basic.testSmJunctionBasicFalse();
}

TEST(BasicU32, True) {
    FppTest::SmInstanceJunction::BasicU32 basicU32("basicU32");
    basicU32.testTrue();
}

TEST(BasicU32, False) {
    FppTest::SmInstanceJunction::BasicU32 basicU32("basicU32");
    basicU32.testFalse();
}

TEST(InputPairU16U32, S1True) {
    FppTest::SmInstanceJunction::InputPairU16U32 inputPair("inputPair");
    inputPair.testS1True();
}

TEST(InputPairU16U32, S1False) {
    FppTest::SmInstanceJunction::InputPairU16U32 inputPair("inputPair");
    inputPair.testS1False();
}

TEST(InputPairU16U32, S2True) {
    FppTest::SmInstanceJunction::InputPairU16U32 inputPair("inputPair");
    inputPair.testS2True();
}

TEST(InputPairU16U32, S2False) {
    FppTest::SmInstanceJunction::InputPairU16U32 inputPair("inputPair");
    inputPair.testS2False();
}

TEST(JunctionToJunction, G1True) {
    FppTest::SmInstanceJunction::JunctionToJunction junctionToJunction("junctionToJunction");
    junctionToJunction.testG1True();
}

TEST(JunctionToJunction, G1FalseG2True) {
    FppTest::SmInstanceJunction::JunctionToJunction junctionToJunction("junctionToJunction");
    junctionToJunction.testG1FalseG2True();
}

TEST(JunctionToJunction, G1FalseG2False) {
    FppTest::SmInstanceJunction::JunctionToJunction junctionToJunction("junctionToJunction");
    junctionToJunction.testG1FalseG2False();
}

TEST(JunctionToJunction, Overflow) {
    FppTest::SmInstanceJunction::JunctionToJunction junctionToJunction("junctionToJunction");
    junctionToJunction.testOverflow();
}

TEST(JunctionToState, True) {
  FppTest::SmInstanceJunction::JunctionToState junctionToState("junctionToState");
  junctionToState.testTrue();
}

TEST(JunctionToState, False) {
  FppTest::SmInstanceJunction::JunctionToState junctionToState("junctionToState");
  junctionToState.testFalse();
}

TEST(Sequence, G1True) {
  FppTest::SmInstanceJunction::Sequence sequence("sequence");
  sequence.testG1True();
}

TEST(Sequence, G1FalseG2True) {
  FppTest::SmInstanceJunction::Sequence sequence("sequence");
  sequence.testG1FalseG2True();
}

TEST(Sequence, G1FalseG2False) {
  FppTest::SmInstanceJunction::Sequence sequence("sequence");
  sequence.testG1FalseG2False();
}

// ----------------------------------------------------------------------
// Main function
// ----------------------------------------------------------------------

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
