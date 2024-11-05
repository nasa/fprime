// ----------------------------------------------------------------------
// main.cpp
// ----------------------------------------------------------------------

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/choice/Basic.hpp"
#include "FppTest/state_machine/internal_instance/choice/BasicU32.hpp"
#include "FppTest/state_machine/internal_instance/choice/ChoiceToChoice.hpp"
#include "FppTest/state_machine/internal_instance/choice/ChoiceToState.hpp"
#include "FppTest/state_machine/internal_instance/choice/InputPairU16U32.hpp"
#include "FppTest/state_machine/internal_instance/choice/Sequence.hpp"
#include "FppTest/state_machine/internal_instance/choice/SequenceU32.hpp"
#include "STest/STest/Random/Random.hpp"

TEST(Basic, BasicTrue) {
    FppTest::SmInstanceChoice::Basic basic("basic");
    basic.testBasicTrue();
}

TEST(Basic, BasicFalse) {
    FppTest::SmInstanceChoice::Basic basic("basic");
    basic.testBasicFalse();
}

TEST(Basic, SmChoiceBasicTrue) {
    FppTest::SmInstanceChoice::Basic basic("basic");
    basic.testSmChoiceBasicTrue();
}

TEST(Basic, SmChoiceBasicFalse) {
    FppTest::SmInstanceChoice::Basic basic("basic");
    basic.testSmChoiceBasicFalse();
}

TEST(BasicU32, True) {
    FppTest::SmInstanceChoice::BasicU32 basicU32("basicU32");
    basicU32.testTrue();
}

TEST(BasicU32, False) {
    FppTest::SmInstanceChoice::BasicU32 basicU32("basicU32");
    basicU32.testFalse();
}

TEST(InputPairU16U32, S1True) {
    FppTest::SmInstanceChoice::InputPairU16U32 inputPair("inputPair");
    inputPair.testS1True();
}

TEST(InputPairU16U32, S1False) {
    FppTest::SmInstanceChoice::InputPairU16U32 inputPair("inputPair");
    inputPair.testS1False();
}

TEST(InputPairU16U32, S2True) {
    FppTest::SmInstanceChoice::InputPairU16U32 inputPair("inputPair");
    inputPair.testS2True();
}

TEST(InputPairU16U32, S2False) {
    FppTest::SmInstanceChoice::InputPairU16U32 inputPair("inputPair");
    inputPair.testS2False();
}

TEST(ChoiceToChoice, G1True) {
    FppTest::SmInstanceChoice::ChoiceToChoice choiceToChoice("choiceToChoice");
    choiceToChoice.testG1True();
}

TEST(ChoiceToChoice, G1FalseG2True) {
    FppTest::SmInstanceChoice::ChoiceToChoice choiceToChoice("choiceToChoice");
    choiceToChoice.testG1FalseG2True();
}

TEST(ChoiceToChoice, G1FalseG2False) {
    FppTest::SmInstanceChoice::ChoiceToChoice choiceToChoice("choiceToChoice");
    choiceToChoice.testG1FalseG2False();
}

TEST(ChoiceToChoice, Overflow) {
    FppTest::SmInstanceChoice::ChoiceToChoice choiceToChoice("choiceToChoice");
    choiceToChoice.testOverflow();
}

TEST(ChoiceToState, True) {
    FppTest::SmInstanceChoice::ChoiceToState choiceToState("choiceToState");
    choiceToState.testTrue();
}

TEST(ChoiceToState, False) {
    FppTest::SmInstanceChoice::ChoiceToState choiceToState("choiceToState");
    choiceToState.testFalse();
}

TEST(Sequence, G1True) {
    FppTest::SmInstanceChoice::Sequence sequence("sequence");
    sequence.testG1True();
}

TEST(Sequence, G1FalseG2True) {
    FppTest::SmInstanceChoice::Sequence sequence("sequence");
    sequence.testG1FalseG2True();
}

TEST(Sequence, G1FalseG2False) {
    FppTest::SmInstanceChoice::Sequence sequence("sequence");
    sequence.testG1FalseG2False();
}

TEST(SequenceU32, G1True) {
    FppTest::SmInstanceChoice::SequenceU32 sequenceU32("sequenceU32");
    sequenceU32.testG1True();
}

TEST(SequenceU32, G1FalseG2True) {
    FppTest::SmInstanceChoice::SequenceU32 sequenceU32("sequenceU32");
    sequenceU32.testG1FalseG2True();
}

TEST(SequenceU32, G1FalseG2False) {
    FppTest::SmInstanceChoice::SequenceU32 sequenceU32("sequenceU32");
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
