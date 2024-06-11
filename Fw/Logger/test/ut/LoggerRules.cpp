/**
 * LoggerRules.cpp:
 *
 * This file specifies Rule classes for testing of the Fw::Logger. These rules can then be used by the main testing
 * program to test the code.
 *
 * Logging rules:
 *
 * 1. a logger can be registered at any time.
 * 2. NULL loggers discard log calls
 * 3. if a valid logger is registered, the log message is called
 *
 * @author mstarch
 */
#include "Fw/Logger/test/ut/LoggerRules.hpp"

namespace LoggerRules {

    // Constructor
    Register::Register(const Fw::String& name) : STest::Rule<MockLogging::FakeLogger>(name.toChar()) {}

    // Check for registration, always allowed
    bool Register::precondition(const MockLogging::FakeLogger& truth) {
        return true;
    }

    // Register NULL or truth as the system logger
    void Register::action(MockLogging::FakeLogger& truth) {
        // Select a registration value: 1 -> logger, 0 -> NULL
        NATIVE_INT_TYPE random = STest::Pick::lowerUpper(0, 1);
        if (random == 1) {
            Fw::Logger::registerLogger(&truth);
            truth.s_current = &truth;
        }
        else {
            Fw::Logger::registerLogger(nullptr);
            truth.s_current = nullptr;
        }
        ASSERT_EQ(truth.s_current, Fw::Logger::s_current_logger);
    }

    // Constructor
    LogGood::LogGood(const Fw::String& name) : STest::Rule<MockLogging::FakeLogger>(name.toChar()) {}

    // Check for logging, only when not NULL
    bool LogGood::precondition(const MockLogging::FakeLogger& truth) {
        return truth.s_current != nullptr;
    }

    // Log valid messages
    void LogGood::action(MockLogging::FakeLogger& truth) {
        NATIVE_INT_TYPE random = STest::Pick::lowerUpper(0, 10);
        NATIVE_INT_TYPE ra[10];
        for (int i = 0; i < 10; ++i) {
            ra[i] = STest::Pick::lowerUpper(0, 0xffffffff);
        }
        
        switch (random) {
            case 0:
                Fw::Logger::logMsg("No args");
                truth.check("No args");
                break;
            case 1:
                Fw::Logger::logMsg("One arg: %lu", ra[0]);
                truth.check("One arg: %lu", ra[0]);
                break;
            case 2:
                Fw::Logger::logMsg("Two arg: %lu", ra[0], ra[1]);
                truth.check("Two arg: %lu", ra[0], ra[1]);
                break;
            case 3:
                Fw::Logger::logMsg("Three arg: %lu", ra[0], ra[1], ra[2]);
                truth.check("Three arg: %lu", ra[0], ra[1], ra[2]);
                break;
            case 4:
                Fw::Logger::logMsg("Four arg: %lu", ra[0], ra[1], ra[2], ra[3]);
                truth.check("Four arg: %lu", ra[0], ra[1], ra[2], ra[3]);
                break;
            case 5:
                Fw::Logger::logMsg("Five arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4]);
                truth.check("Five arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4]);
                break;
            case 6:
                Fw::Logger::logMsg("Six arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5]);
                truth.check("Six arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5]);
                break;
            case 7:
                Fw::Logger::logMsg("Seven arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5], ra[6]);
                truth.check("Seven arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5], ra[6]);
                break;
            case 8:
                Fw::Logger::logMsg("Eight arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5], ra[6], ra[7]);
                truth.check("Eight arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5], ra[6], ra[7]);
                break;
            case 9:
                Fw::Logger::logMsg("Nine arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5], ra[6], ra[7], ra[8]);
                truth.check("Nine arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5], ra[6], ra[7], ra[8]);
                break;
            case 10:
                Fw::Logger::logMsg("Ten arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5], ra[6], ra[7], ra[8], ra[9]);
                truth.check("Ten arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5], ra[6], ra[7], ra[8], ra[9]);
                break;

            default:
                ASSERT_EQ(0, 1);
        }
        truth.reset();
    }

    // Constructor
    LogBad::LogBad(const Fw::String& name) : STest::Rule<MockLogging::FakeLogger>(name.toChar()) {}

    // Check for logging, only when not NULL
    bool LogBad::precondition(const MockLogging::FakeLogger& truth) {
        return truth.s_current == nullptr;
    }

    // Log valid messages
    void LogBad::action(MockLogging::FakeLogger& truth) {
        NATIVE_INT_TYPE random = STest::Pick::lowerUpper(0, 10);
        NATIVE_INT_TYPE ra[10];
        for (int i = 0; i < 10; ++i) {
            ra[i] = STest::Pick::lowerUpper(0, 0xffffffff);
        }

        switch (random) {
            case 0:
                Fw::Logger::logMsg("No args");
                truth.check(nullptr);
                break;
            case 1:
                Fw::Logger::logMsg("One arg: %lu", ra[0]);
                truth.check(nullptr);
                break;
            case 2:
                Fw::Logger::logMsg("Two arg: %lu", ra[0], ra[1]);
                truth.check(nullptr);
                break;
            case 3:
                Fw::Logger::logMsg("Three arg: %lu", ra[0], ra[1], ra[2]);
                truth.check(nullptr);
                break;
            case 4:
                Fw::Logger::logMsg("Four arg: %lu", ra[0], ra[1], ra[2], ra[3]);
                truth.check(nullptr);
                break;
            case 5:
                Fw::Logger::logMsg("Five arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4]);
                truth.check(nullptr);
                break;
            case 6:
                Fw::Logger::logMsg("Six arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5]);
                truth.check(nullptr);
                break;
            case 7:
                Fw::Logger::logMsg("Seven arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5], ra[6]);
                truth.check(nullptr);
                break;
            case 8:
                Fw::Logger::logMsg("Eight arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5], ra[6], ra[7]);
                truth.check(nullptr);
                break;
            case 9:
                Fw::Logger::logMsg("Nine arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5], ra[6], ra[7], ra[8]);
                truth.check(nullptr);
                break;
            case 10:
                Fw::Logger::logMsg("Ten arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5], ra[6], ra[7], ra[8], ra[9]);
                truth.check(nullptr);
                break;
            default:
                ASSERT_EQ(0, 1);
        }
        truth.reset();
    }
};
