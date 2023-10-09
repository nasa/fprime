// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include <gtest/gtest.h>

#include "Fw/Test/UnitTest.hpp"
#include "GenerateFrames.hpp"
#include "Os/Log.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "STest/Scenario/Scenario.hpp"
#include "SendBuffer.hpp"
#include "DeframerTester.hpp"

#define STEP_COUNT 10000

// Uncomment the following line to turn on OS logging
//Os::Log logger;

// ----------------------------------------------------------------------
// Static helper functions
// ----------------------------------------------------------------------

//! Run a random scenario
static void runRandomScenario(Svc::DeframerTester::InputMode::t inputMode) {
    Svc::DeframerTester tester(Svc::DeframerTester::InputMode::PUSH);

    // Create rules, and assign them into the array
    Svc::GenerateFrames generateFrames;
    Svc::SendBuffer sendBuffer;

    // Setup a list of rules to choose from
    STest::Rule<Svc::DeframerTester>* rules[] = {
            &generateFrames,
            &sendBuffer
    };
    // Construct the random scenario and run it with the defined bounds
    STest::RandomScenario<Svc::DeframerTester> random(
        "Random Rules",
        rules,
        FW_NUM_ARRAY_ELEMENTS(rules)
    );

    // Setup a bounded scenario to run rules a set number of times
    STest::BoundedScenario<Svc::DeframerTester> bounded(
        "Bounded Random Rules Scenario",
        random,
        STEP_COUNT
    );
    // Run!
    const U32 numSteps = bounded.run(tester);
    printf("Ran %u steps.\n", numSteps);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

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
    Svc::DeframerTester tester(Svc::DeframerTester::InputMode::PUSH);
    Svc::GenerateFrames().apply(tester);
    Svc::SendBuffer().apply(tester);
}

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
    Svc::DeframerTester tester(Svc::DeframerTester::InputMode::POLL);
    Svc::GenerateFrames().apply(tester);
    Svc::SendBuffer().apply(tester);
}

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
    runRandomScenario(Svc::DeframerTester::InputMode::PUSH);
}

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
    runRandomScenario(Svc::DeframerTester::InputMode::POLL);
}

TEST(Error, SizeOverflow) {
    COMMENT("Test handling of size overflow in F Prime deframing protocol");
    Svc::DeframerTester tester(Svc::DeframerTester::InputMode::PUSH);
    tester.sizeOverflow();
}

// ----------------------------------------------------------------------
// Main function
// ----------------------------------------------------------------------

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
