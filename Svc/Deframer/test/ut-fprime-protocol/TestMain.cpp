// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"
#include <Fw/Test/UnitTest.hpp>
#include <Os/Log.hpp>
#include <STest/Scenario/BoundedScenario.hpp>
#include <STest/Scenario/RandomScenario.hpp>
#include <STest/Scenario/Scenario.hpp>
#include <Svc/Deframer/test/ut-fprime-protocol/DeframerRules.hpp>
#include <gtest/gtest.h>

#define STEP_COUNT 10000

// Uncomment the following line to turn on OS logging
Os::Log logger;
// This is off by default due to the large amount of checksum error output

TEST(Nominal, BasicPush) {
    COMMENT("Send one buffer to the deframer, simulating an active driver (push)");
    REQUIREMENT("SVC-DEFRAMER-001");
    REQUIREMENT("SVC-DEFRAMER-002");
    REQUIREMENT("SVC-DEFRAMER-003");
    REQUIREMENT("SVC-DEFRAMER-004");
    REQUIREMENT("SVC-DEFRAMER-007");
    REQUIREMENT("SVC-DEFRAMER-008");
    REQUIREMENT("SVC-DEFRAMER-009");
    REQUIREMENT("SVC-DEFRAMER-010");
    Svc::Tester tester(Svc::Tester::InputMode::PUSH);
    Svc::GenerateFrames generateFrames;
    Svc::SendBuffer sendBuffer;

    generateFrames.apply(tester);
    sendBuffer.apply(tester);
}

#if 0
TEST(Nominal, BasicPoll) {
    COMMENT("Send one buffer to the deframer, simulating a passive driver (poll)");
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
    Svc::GenerateFrames generateFrames();
    Svc::SendBuffer sendBuffer();

    setup.apply(tester);
    send.apply(tester);
}
#endif

TEST(Nominal, RandomPush) {
    COMMENT("Send random buffers to the deframer, simulating an active driver (push)");
    REQUIREMENT("SVC-DEFRAMER-001");
    REQUIREMENT("SVC-DEFRAMER-002");
    REQUIREMENT("SVC-DEFRAMER-003");
    REQUIREMENT("SVC-DEFRAMER-004");
    REQUIREMENT("SVC-DEFRAMER-007");
    REQUIREMENT("SVC-DEFRAMER-008");
    REQUIREMENT("SVC-DEFRAMER-009");
    REQUIREMENT("SVC-DEFRAMER-010");
    Svc::Tester tester(Svc::Tester::InputMode::PUSH);

    // Create rules, and assign them into the array
    Svc::GenerateFrames generateFrames;
    Svc::SendBuffer sendBuffer;

    // Setup a list of rules to choose from
    STest::Rule<Svc::Tester>* rules[] = {
            &generateFrames,
            &sendBuffer
    };
    // Construct the random scenario and run it with the defined bounds
    STest::RandomScenario<Svc::Tester> random(
        "Random Rules",
        rules,
        FW_NUM_ARRAY_ELEMENTS(rules)
    );

    // Setup a bounded scenario to run rules a set number of times
    STest::BoundedScenario<Svc::Tester> bounded(
        "Bounded Random Rules Scenario",
        random,
        STEP_COUNT
    );
    // Run!
    const U32 numSteps = bounded.run(tester);
    printf("Ran %u steps.\n", numSteps);
}

#if 0
TEST(Nominal, RandomPoll) {
    COMMENT("Send random buffers to the deframer, simulating a passive driver (poll)");
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
    Svc::GenerateFrames generateFrames();
    Svc::SendBuffer sendBuffer();

    // Setup a list of rules to choose from
    STest::Rule<Svc::Tester>* rules[] = {
            &generateFrames,
            &sendBuffer
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

TEST(Error, SizeOverflow) {
    COMMENT("Test handling of size overflow in F Prime deframing protocol");
    Svc::Tester tester(Svc::Tester::InputMode::PUSH);
    tester.sizeOverflow();
}

// TODO: Add a test for size larger than ring buffer size

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
