// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"
#include <Fw/Test/UnitTest.hpp>
#include <Os/Log.hpp>
#include <STest/Scenario/BoundedScenario.hpp>
#include <STest/Scenario/RandomScenario.hpp>
#include <STest/Scenario/Scenario.hpp>
#include <Svc/GroundInterface/test/ut/GroundInterfaceRules.hpp>
#include <gtest/gtest.h>

#define STEP_COUNT 10

// Uncomment the following line to turn on OS logging
Os::Log logger;
// This is off by default due to the large amount of checksum error output

#if 0
TEST(Nominal, BasicUplink) {
    COMMENT("Send one buffer to the deframer, simulating an active driver");
    REQUIREMENT("SVC-DEFRAMER-001");
    REQUIREMENT("SVC-DEFRAMER-002");
    REQUIREMENT("SVC-DEFRAMER-003");
    REQUIREMENT("SVC-DEFRAMER-004");
    REQUIREMENT("SVC-DEFRAMER-007");
    REQUIREMENT("SVC-DEFRAMER-008");
    REQUIREMENT("SVC-DEFRAMER-009");
    REQUIREMENT("SVC-DEFRAMER-010");
    Svc::Tester tester(false);
    Svc::RandomizeRule setup("Randomize");
    Svc::SendAvailableRule send("Uplink Rule");

    setup.apply(tester);
    send.apply(tester);
}
#endif

#if 0
// TODO: Fix address boundary error
TEST(Nominal, BasicPollUplink) {
    COMMENT("Send one buffer to the deframer, simulating a passive driver");
    REQUIREMENT("SVC-DEFRAMER-001");
    REQUIREMENT("SVC-DEFRAMER-002");
    REQUIREMENT("SVC-DEFRAMER-003");
    REQUIREMENT("SVC-DEFRAMER-005");
    REQUIREMENT("SVC-DEFRAMER-006");
    REQUIREMENT("SVC-DEFRAMER-007");
    REQUIREMENT("SVC-DEFRAMER-008");
    REQUIREMENT("SVC-DEFRAMER-009");
    REQUIREMENT("SVC-DEFRAMER-010");
    Svc::Tester tester(true);
    Svc::RandomizeRule setup("Randomize");
    Svc::SendAvailableRule send("Uplink Rule");

    setup.apply(tester);
    send.apply(tester);
}
#endif

/**
 * A random hopper for rules. Apply STEP_COUNT times.
 */
TEST(Nominal, RandomizedDeframer) {
    COMMENT("Send random buffers to the deframer, simulating an active driver");
    REQUIREMENT("SVC-DEFRAMER-001");
    REQUIREMENT("SVC-DEFRAMER-002");
    REQUIREMENT("SVC-DEFRAMER-003");
    REQUIREMENT("SVC-DEFRAMER-004");
    REQUIREMENT("SVC-DEFRAMER-007");
    REQUIREMENT("SVC-DEFRAMER-008");
    REQUIREMENT("SVC-DEFRAMER-009");
    REQUIREMENT("SVC-DEFRAMER-010");
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

#if 0
TEST(Nominal, RandomizedPollingDeframer) {
    COMMENT("Send random buffers to the deframer, simulating a passive driver");
    REQUIREMENT("SVC-DEFRAMER-001");
    REQUIREMENT("SVC-DEFRAMER-002");
    REQUIREMENT("SVC-DEFRAMER-003");
    REQUIREMENT("SVC-DEFRAMER-005");
    REQUIREMENT("SVC-DEFRAMER-006");
    REQUIREMENT("SVC-DEFRAMER-007");
    REQUIREMENT("SVC-DEFRAMER-008");
    REQUIREMENT("SVC-DEFRAMER-009");
    REQUIREMENT("SVC-DEFRAMER-010");
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
#endif

#if 0
TEST(Error, SizeTooLarge) {
    COMMENT("Test handling of size overflow in F Prime deframing protocol");
    Svc::Tester tester(false);
    tester.sizeTooLarge();
}
#endif

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
