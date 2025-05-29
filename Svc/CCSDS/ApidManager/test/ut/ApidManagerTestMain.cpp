// ======================================================================
// \title  ApidManagerTestMain.cpp
// \author chammard
// \brief  cpp file for ApidManager component test main function
// ======================================================================

#include "ApidManagerTester.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"

TEST(ApidManager, getExistingSeqCount) {
    Svc::CCSDS::ApidManagerTester tester;
    Svc::CCSDS::ApidManagerTester::GetExistingSeqCount rule1;
    rule1.apply(tester);
}

// Randomized sequence of conditioned take/release/lock/unlock
TEST(ApidManager, RandomizedInterfaceTesting) {

    Svc::CCSDS::ApidManagerTester tester;
    
    Svc::CCSDS::ApidManagerTester::GetExistingSeqCount rule1;
    Svc::CCSDS::ApidManagerTester::GetNewSeqCountOk rule2;
    Svc::CCSDS::ApidManagerTester::GetNewSeqCountTableFull rule3;
    // rule1.apply(tester);

    // Place these rules into a list of rules
    STest::Rule<Svc::CCSDS::ApidManagerTester>* rules[] = {
            &rule1,
            &rule2,
            &rule3
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
    // add one run of unlock for safe destruction
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
