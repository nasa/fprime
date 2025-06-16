// ======================================================================
// \title  ApidManagerTestMain.cpp
// \author thomas-bc
// \brief  cpp file for ApidManager component test main function
// ======================================================================

#include "ApidManagerTester.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"

TEST(ApidManager, GetSequenceCounts) {
    Svc::CCSDS::ApidManagerTester tester;
    Svc::CCSDS::ApidManagerTester::GetExistingSeqCount getExistingSeqCount;
    Svc::CCSDS::ApidManagerTester::GetNewSeqCountOk getNewSeqCountOk;
    getExistingSeqCount.apply(tester);
    getNewSeqCountOk.apply(tester);
}

TEST(ApidManager, ValidateSequenceCounts) {
    Svc::CCSDS::ApidManagerTester tester;
    Svc::CCSDS::ApidManagerTester::ValidateSeqCountOk validateSeqCountOkRule;
    Svc::CCSDS::ApidManagerTester::ValidateSeqCountFailure validateSeqCountFailureRule;
    validateSeqCountOkRule.apply(tester);
    validateSeqCountFailureRule.apply(tester);
}

// Randomized testing
TEST(ApidManager, RandomizedTesting) {

    Svc::CCSDS::ApidManagerTester tester;
    
    Svc::CCSDS::ApidManagerTester::GetExistingSeqCount getExistingSeqCountRule;
    Svc::CCSDS::ApidManagerTester::GetNewSeqCountOk getNewSeqCountOkRule;
    Svc::CCSDS::ApidManagerTester::GetNewSeqCountTableFull getNewSeqCountTableFullRule;
    Svc::CCSDS::ApidManagerTester::ValidateSeqCountOk validateSeqCountOkRule;
    Svc::CCSDS::ApidManagerTester::ValidateSeqCountFailure validateSeqCountFailureRule;

    // Place these rules into a list of rules
    STest::Rule<Svc::CCSDS::ApidManagerTester>* rules[] = {
            &getExistingSeqCountRule,
            &getNewSeqCountOkRule,
            &getNewSeqCountTableFullRule,
            &validateSeqCountOkRule,
            &validateSeqCountFailureRule
    };

    // Take the rules and place them into a random scenario
    STest::RandomScenario<Svc::CCSDS::ApidManagerTester> random(
            "Random Rules",
            rules,
            FW_NUM_ARRAY_ELEMENTS(rules)
    );

    // Create a bounded scenario wrapping the random scenario
    STest::BoundedScenario<Svc::CCSDS::ApidManagerTester> bounded(
            "Bounded Random Rules Scenario",
            random,
            1000
    );
    // Run!
    const U32 numSteps = bounded.run(tester);
    printf("Ran %u steps.\n", numSteps);
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
