// ----------------------------------------------------------------------
// main.cpp
// ----------------------------------------------------------------------

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal/choice/Basic.hpp"
#include "FppTest/state_machine/internal/choice/BasicU32.hpp"
#include "FppTest/state_machine/internal/choice/ChoiceToChoice.hpp"
#include "FppTest/state_machine/internal/choice/ChoiceToState.hpp"
#include "FppTest/state_machine/internal/choice/InputPairU16U32.hpp"
#include "FppTest/state_machine/internal/choice/Sequence.hpp"
#include "FppTest/state_machine/internal/choice/SequenceU32.hpp"
#include "STest/STest/Random/Random.hpp"

TEST(Basic, True) {
    FppTest::SmChoice::Basic basic;
    basic.testTrue();
}

TEST(Basic, False) {
    FppTest::SmChoice::Basic basic;
    basic.testFalse();
}

TEST(BasicU32, True) {
    FppTest::SmChoice::BasicU32 basicU32;
    basicU32.testTrue();
}

TEST(BasicU32, False) {
    FppTest::SmChoice::BasicU32 basicU32;
    basicU32.testFalse();
}

TEST(InputPairU16U32, S1True) {
    FppTest::SmChoice::InputPairU16U32 inputPair;
    inputPair.testS1True();
}

TEST(InputPairU16U32, S1False) {
    FppTest::SmChoice::InputPairU16U32 inputPair;
    inputPair.testS1False();
}

TEST(InputPairU16U32, S2True) {
    FppTest::SmChoice::InputPairU16U32 inputPair;
    inputPair.testS2True();
}

TEST(InputPairU16U32, S2False) {
    FppTest::SmChoice::InputPairU16U32 inputPair;
    inputPair.testS2False();
}

TEST(ChoiceToChoice, G1True) {
    FppTest::SmChoice::ChoiceToChoice choiceToChoice;
    choiceToChoice.testG1True();
}

TEST(ChoiceToChoice, G1FalseG2True) {
    FppTest::SmChoice::ChoiceToChoice choiceToChoice;
    choiceToChoice.testG1FalseG2True();
}

TEST(ChoiceToChoice, G1FalseG2False) {
    FppTest::SmChoice::ChoiceToChoice choiceToChoice;
    choiceToChoice.testG1FalseG2False();
}

TEST(ChoiceToState, True) {
    FppTest::SmChoice::ChoiceToState choiceToState;
    choiceToState.testTrue();
}

TEST(ChoiceToState, False) {
    FppTest::SmChoice::ChoiceToState choiceToState;
    choiceToState.testFalse();
}

TEST(Sequence, G1True) {
    FppTest::SmChoice::Sequence sequence;
    sequence.testG1True();
}

TEST(Sequence, G1FalseG2True) {
    FppTest::SmChoice::Sequence sequence;
    sequence.testG1FalseG2True();
}

TEST(Sequence, G1FalseG2False) {
    FppTest::SmChoice::Sequence sequence;
    sequence.testG1FalseG2False();
}

TEST(SequenceU32, G1True) {
    FppTest::SmChoice::SequenceU32 sequenceU32;
    sequenceU32.testG1True();
}

TEST(SequenceU32, G1FalseG2True) {
    FppTest::SmChoice::SequenceU32 sequenceU32;
    sequenceU32.testG1FalseG2True();
}

TEST(SequenceU32, G1FalseG2False) {
    FppTest::SmChoice::SequenceU32 sequenceU32;
    sequenceU32.testG1FalseG2False();
}

// ----------------------------------------------------------------------
// Main function
// ----------------------------------------------------------------------

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
