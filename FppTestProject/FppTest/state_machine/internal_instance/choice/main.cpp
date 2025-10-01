// ----------------------------------------------------------------------
// main.cpp
// ----------------------------------------------------------------------

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/choice/BasicTester.hpp"
#include "FppTest/state_machine/internal_instance/choice/BasicU32Tester.hpp"
#include "FppTest/state_machine/internal_instance/choice/ChoiceToChoiceTester.hpp"
#include "FppTest/state_machine/internal_instance/choice/ChoiceToStateTester.hpp"
#include "FppTest/state_machine/internal_instance/choice/InputPairU16U32Tester.hpp"
#include "FppTest/state_machine/internal_instance/choice/SequenceTester.hpp"
#include "FppTest/state_machine/internal_instance/choice/SequenceU32Tester.hpp"
#include "STest/STest/Random/Random.hpp"

TEST(Basic, BasicTrue) {
    FppTest::SmInstanceChoice::BasicTester basic("basic");
    basic.testBasicTrue();
}

TEST(Basic, BasicFalse) {
    FppTest::SmInstanceChoice::BasicTester basic("basic");
    basic.testBasicFalse();
}

TEST(Basic, SmChoiceBasicTrue) {
    FppTest::SmInstanceChoice::BasicTester basic("basic");
    basic.testSmChoiceBasicTrue();
}

TEST(Basic, SmChoiceBasicFalse) {
    FppTest::SmInstanceChoice::BasicTester basic("basic");
    basic.testSmChoiceBasicFalse();
}

TEST(BasicU32, True) {
    FppTest::SmInstanceChoice::BasicU32Tester basicU32("basicU32");
    basicU32.testTrue();
}

TEST(BasicU32, False) {
    FppTest::SmInstanceChoice::BasicU32Tester basicU32("basicU32");
    basicU32.testFalse();
}

TEST(InputPairU16U32, S1True) {
    FppTest::SmInstanceChoice::InputPairU16U32Tester inputPair("inputPair");
    inputPair.testS1True();
}

TEST(InputPairU16U32, S1False) {
    FppTest::SmInstanceChoice::InputPairU16U32Tester inputPair("inputPair");
    inputPair.testS1False();
}

TEST(InputPairU16U32, S2True) {
    FppTest::SmInstanceChoice::InputPairU16U32Tester inputPair("inputPair");
    inputPair.testS2True();
}

TEST(InputPairU16U32, S2False) {
    FppTest::SmInstanceChoice::InputPairU16U32Tester inputPair("inputPair");
    inputPair.testS2False();
}

TEST(ChoiceToChoice, G1True) {
    FppTest::SmInstanceChoice::ChoiceToChoiceTester choiceToChoice("choiceToChoice");
    choiceToChoice.testG1True();
}

TEST(ChoiceToChoice, G1FalseG2True) {
    FppTest::SmInstanceChoice::ChoiceToChoiceTester choiceToChoice("choiceToChoice");
    choiceToChoice.testG1FalseG2True();
}

TEST(ChoiceToChoice, G1FalseG2False) {
    FppTest::SmInstanceChoice::ChoiceToChoiceTester choiceToChoice("choiceToChoice");
    choiceToChoice.testG1FalseG2False();
}

TEST(ChoiceToChoice, Overflow) {
    FppTest::SmInstanceChoice::ChoiceToChoiceTester choiceToChoice("choiceToChoice");
    choiceToChoice.testOverflow();
}

TEST(ChoiceToState, True) {
    FppTest::SmInstanceChoice::ChoiceToStateTester choiceToState("choiceToState");
    choiceToState.testTrue();
}

TEST(ChoiceToState, False) {
    FppTest::SmInstanceChoice::ChoiceToStateTester choiceToState("choiceToState");
    choiceToState.testFalse();
}

TEST(Sequence, G1True) {
    FppTest::SmInstanceChoice::SequenceTester sequence("sequence");
    sequence.testG1True();
}

TEST(Sequence, G1FalseG2True) {
    FppTest::SmInstanceChoice::SequenceTester sequence("sequence");
    sequence.testG1FalseG2True();
}

TEST(Sequence, G1FalseG2False) {
    FppTest::SmInstanceChoice::SequenceTester sequence("sequence");
    sequence.testG1FalseG2False();
}

TEST(SequenceU32, G1True) {
    FppTest::SmInstanceChoice::SequenceU32Tester sequenceU32("sequenceU32");
    sequenceU32.testG1True();
}

TEST(SequenceU32, G1FalseG2True) {
    FppTest::SmInstanceChoice::SequenceU32Tester sequenceU32("sequenceU32");
    sequenceU32.testG1FalseG2True();
}

TEST(SequenceU32, G1FalseG2False) {
    FppTest::SmInstanceChoice::SequenceU32Tester sequenceU32("sequenceU32");
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
