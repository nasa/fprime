// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"
#include <Os/Log.hpp>
#include <STest/Scenario/BoundedScenario.hpp>
#include <STest/Scenario/RandomScenario.hpp>
#include <STest/Scenario/Scenario.hpp>
#include <Svc/GroundInterface/test/ut/GroundInterfaceRules.hpp>
#include <gtest/gtest.h>

#define STEP_COUNT 10000

// Uncomment the following line to turn on OS logging
// Os::Log logger;
// This is off by default due to the large amount of checksum error output

/**
 * A random hopper for rules. Apply STEP_COUNT times.
 */
TEST(Nominal, RandomizedDeframer) {
    Svc::Tester tester;

    // Create rules, and assign them into the array
    Svc::RandomizeRule randomize("Randomize");
    Svc::SendAvailableRule sendup("Send");

    // Setup a list of rules to choose from
    STest::Rule<Svc::Tester>* rules[] = {
            &randomize,
            &sendup
    };
    // Construct the random scenario and run it with the defined bounds
    STest::RandomScenario<Svc::Tester> random("Random Rules", rules, FW_NUM_ARRAY_ELEMENTS(rules));

    // Setup a bounded scenario to run rules a set number of times
    STest::BoundedScenario<Svc::Tester> bounded("Bounded Random Rules Scenario", random, STEP_COUNT);
    // Run!
    const U32 numSteps = bounded.run(tester);
    printf("Ran %u steps.\n", numSteps);
}

TEST(Nominal, RandomizedPollingDeframer) {
    Svc::Tester tester(true);

    // Create rules, and assign them into the array
    Svc::RandomizeRule randomize("Randomize");
    Svc::SendAvailableRule sendup("Send");

    // Setup a list of rules to choose from
    STest::Rule<Svc::Tester>* rules[] = {
            &randomize,
            &sendup
    };
    // Construct the random scenario and run it with the defined bounds
    STest::RandomScenario<Svc::Tester> random("Random Rules", rules, FW_NUM_ARRAY_ELEMENTS(rules));

    // Setup a bounded scenario to run rules a set number of times
    STest::BoundedScenario<Svc::Tester> bounded("Bounded Random Rules Scenario", random, STEP_COUNT);
    // Run!
    const U32 numSteps = bounded.run(tester);
    printf("Ran %u steps.\n", numSteps);
}

TEST(Nominal, BasicUplink) {
    Svc::Tester tester(false);
    Svc::RandomizeRule setup("Randomize");
    Svc::SendAvailableRule send("Uplink Rule");

    setup.apply(tester);
    send.apply(tester);
}

TEST(Nominal, BasicPollUplink) {
    Svc::Tester tester(false);
    Svc::RandomizeRule setup("Randomize");
    Svc::SendAvailableRule send("Uplink Rule");

    setup.apply(tester);
    send.apply(tester);
}

TEST(Nominal, CommandResponse) {
    Svc::Tester tester(false);
    tester.commandResponse();
}

TEST(Error, CommandPacketTooLarge) {
    Svc::Tester tester(false);
    tester.commandPacketTooLarge();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
