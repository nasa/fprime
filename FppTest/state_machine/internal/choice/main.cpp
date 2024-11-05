// ----------------------------------------------------------------------
// main.cpp
// ----------------------------------------------------------------------

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal/choice/Basic.hpp"
#include "FppTest/state_machine/internal/choice/BasicU32.hpp"
#include "FppTest/state_machine/internal/choice/InputPairU16U32.hpp"
#include "FppTest/state_machine/internal/choice/JunctionToJunction.hpp"
#include "FppTest/state_machine/internal/choice/JunctionToState.hpp"
#include "FppTest/state_machine/internal/choice/Sequence.hpp"
#include "FppTest/state_machine/internal/choice/SequenceU32.hpp"
#include "STest/STest/Random/Random.hpp"

TEST(Basic, True) {
  FppTest::SmJunction::Basic basic;
  basic.testTrue();
}

TEST(Basic, False) {
  FppTest::SmJunction::Basic basic;
  basic.testFalse();
}

TEST(BasicU32, True) {
  FppTest::SmJunction::BasicU32 basicU32;
  basicU32.testTrue();
}

TEST(BasicU32, False) {
  FppTest::SmJunction::BasicU32 basicU32;
  basicU32.testFalse();
}

TEST(InputPairU16U32, S1True) {
  FppTest::SmJunction::InputPairU16U32 inputPair;
  inputPair.testS1True();
}

TEST(InputPairU16U32, S1False) {
  FppTest::SmJunction::InputPairU16U32 inputPair;
  inputPair.testS1False();
}

TEST(InputPairU16U32, S2True) {
  FppTest::SmJunction::InputPairU16U32 inputPair;
  inputPair.testS2True();
}

TEST(InputPairU16U32, S2False) {
  FppTest::SmJunction::InputPairU16U32 inputPair;
  inputPair.testS2False();
}

TEST(JunctionToJunction, G1True) {
  FppTest::SmJunction::JunctionToJunction junctionToJunction;
  junctionToJunction.testG1True();
}

TEST(JunctionToJunction, G1FalseG2True) {
  FppTest::SmJunction::JunctionToJunction junctionToJunction;
  junctionToJunction.testG1FalseG2True();
}

TEST(JunctionToJunction, G1FalseG2False) {
  FppTest::SmJunction::JunctionToJunction junctionToJunction;
  junctionToJunction.testG1FalseG2False();
}

TEST(JunctionToState, True) {
  FppTest::SmJunction::JunctionToState junctionToState;
  junctionToState.testTrue();
}

TEST(JunctionToState, False) {
  FppTest::SmJunction::JunctionToState junctionToState;
  junctionToState.testFalse();
}

TEST(Sequence, G1True) {
  FppTest::SmJunction::Sequence sequence;
  sequence.testG1True();
}

TEST(Sequence, G1FalseG2True) {
  FppTest::SmJunction::Sequence sequence;
  sequence.testG1FalseG2True();
}

TEST(Sequence, G1FalseG2False) {
  FppTest::SmJunction::Sequence sequence;
  sequence.testG1FalseG2False();
}

TEST(SequenceU32, G1True) {
  FppTest::SmJunction::SequenceU32 sequenceU32;
  sequenceU32.testG1True();
}

TEST(SequenceU32, G1FalseG2True) {
  FppTest::SmJunction::SequenceU32 sequenceU32;
  sequenceU32.testG1FalseG2True();
}

TEST(SequenceU32, G1FalseG2False) {
  FppTest::SmJunction::SequenceU32 sequenceU32;
  sequenceU32.testG1FalseG2False();
}

// ----------------------------------------------------------------------
// Main function
// ----------------------------------------------------------------------

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
