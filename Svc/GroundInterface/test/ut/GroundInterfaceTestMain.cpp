// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "GroundInterfaceTester.hpp"
#include <gtest/gtest.h>
#include <Svc/GroundInterface/test/ut/GroundInterfaceRules.hpp>
#include <STest/Scenario/Scenario.hpp>
#include <STest/Scenario/RandomScenario.hpp>
#include <STest/Scenario/BoundedScenario.hpp>

#define STEP_COUNT 10000

/**
 * A random hopper for rules. Apply STEP_COUNT times.
 */
TEST(Nominal, RandomizedGroundIf) {
    Svc::GroundInterfaceTester tester;

    // Create rules, and assign them into the array
    Svc::RandomizeRule randomize("Randomize");
    Svc::DownlinkRule downlink("Downlink");
    Svc::FileDownlinkRule filedown("File Down");
    Svc::SendAvailableRule sendup("");

    // Setup a list of rules to choose from
    STest::Rule<Svc::GroundInterfaceTester>* rules[] = {
            &randomize,
            &downlink,
            &filedown,
            &sendup
    };
    // Construct the random scenario and run it with the defined bounds
    STest::RandomScenario<Svc::GroundInterfaceTester> random("Random Rules", rules, FW_NUM_ARRAY_ELEMENTS(rules));

    // Setup a bounded scenario to run rules a set number of times
    STest::BoundedScenario<Svc::GroundInterfaceTester> bounded("Bounded Random Rules Scenario", random, STEP_COUNT);
    // Run!
    const U32 numSteps = bounded.run(tester);
    printf("Ran %u steps.\n", numSteps);
}


TEST(Nominal, BasicUplink) {
    Svc::GroundInterfaceTester tester;
    Svc::SendAvailableRule rule("Uplink Rule");
    rule.apply(tester);
}

TEST(Nominal, BasicDownlink) {
    Svc::GroundInterfaceTester tester;
    Svc::DownlinkRule rule("Downlink Rule");
    rule.apply(tester);
}

TEST(Nominal, FileDownlink) {
    Svc::GroundInterfaceTester tester;
    Svc::FileDownlinkRule rule("File Down Rule");
    rule.apply(tester);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
