/**
 * Main.cpp:
 *
 * Setup the GTests for rules-based testing of Fw::Logger and runs these tests.
 *
 *  Created on: May 23, 2019
 *      Author: mstarch
 */
#include <STest/Scenario/Scenario.hpp>
#include <STest/Scenario/RandomScenario.hpp>
#include <STest/Scenario/BoundedScenario.hpp>

#include <Fw/Test/UnitTest.hpp>
#include <Fw/Logger/test/ut/LoggerRules.hpp>
#include <gtest/gtest.h>

#include <cstdio>

#define STEP_COUNT 10000

/**
 * A random hopper for rules. Apply STEP_COUNT times.
 */
TEST(LoggerTests, RandomLoggerTests) {
    MockLogging::FakeLogger logger;

    // Create rules, and assign them into the array
    LoggerRules::Register reg("Register");
    LoggerRules::LogGood log("Log Successfully");
    LoggerRules::LogBad nolog("Log unsuccessfully");

    // Setup a list of rules to choose from
    STest::Rule<MockLogging::FakeLogger>* rules[] = {
            &reg,
            &log,
            &nolog
    };
    // Construct the random scenario and run it with the defined bounds
    STest::RandomScenario<MockLogging::FakeLogger> random("Random Rules", rules,
                                                      FW_NUM_ARRAY_ELEMENTS(rules));

    // Setup a bounded scenario to run rules a set number of times
    STest::BoundedScenario<MockLogging::FakeLogger> bounded("Bounded Random Rules Scenario",
                                                        random, STEP_COUNT);
    // Run!
    const U32 numSteps = bounded.run(logger);
    printf("Ran %u steps.\n", numSteps);
}
/**
 * Test that the most basic logging function works.
 */
TEST(LoggerTests, BasicGoodLogger) {
    // Setup and register logger
    MockLogging::FakeLogger logger;
    Fw::Logger::registerLogger(&logger);
    logger.s_current = &logger;
    // Basic logging
    LoggerRules::LogGood log("Log Successfully");
    log.apply(logger);
}
/**
 * Test that null-logging function works.
 */
TEST(LoggerTests, BasicBadLogger) {
    // Basic discard logging
    MockLogging::FakeLogger logger;
    Fw::Logger::registerLogger(nullptr);
    logger.s_current = nullptr;
    LoggerRules::LogBad log("Log Discarded");
    log.apply(logger);
}

/**
 * Test that registration works. Multiple times, as contains randomness.
 */
TEST(LoggerTests, BasicRegLogger) {
    // Basic discard logging
    MockLogging::FakeLogger logger;
    LoggerRules::Register reg("Register");
    reg.apply(logger);
    reg.apply(logger);
    reg.apply(logger);
    reg.apply(logger);
    reg.apply(logger);
    reg.apply(logger);
    reg.apply(logger);
}


int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
