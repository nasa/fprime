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
#include "Tester.hpp"

#define STEP_COUNT 10000

// Uncomment the following line to turn on OS logging
//Os::Log logger;

// ----------------------------------------------------------------------
// Static helper functions
// ----------------------------------------------------------------------

//! Run a random scenario
static void runRandomScenario() {
    Svc::Tester tester;

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
    Svc::Tester tester;
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
    runRandomScenario();
}

TEST(Error, SizeOverflow) {
    COMMENT("Test handling of size overflow in F Prime deframing protocol");
    Svc::Tester tester;
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
