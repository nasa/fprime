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
    Register::Register(const Fw::EightyCharString& name) : STest::Rule<MockLogging::FakeLogger>(name.m_buf) {}

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
            Fw::Logger::registerLogger(NULL);
            truth.s_current = NULL;
        }
        ASSERT_EQ(truth.s_current, Fw::Logger::s_current_logger);
    }

    // Constructor
    LogGood::LogGood(const Fw::EightyCharString& name) : STest::Rule<MockLogging::FakeLogger>(name.m_buf) {}

    // Check for logging, only when not NULL
    bool LogGood::precondition(const MockLogging::FakeLogger& truth) {
        return truth.s_current != NULL;
    }

    // Log valid messages
    void LogGood::action(MockLogging::FakeLogger& truth) {
        NATIVE_INT_TYPE random = STest::Pick::lowerUpper(0, 6);
        NATIVE_INT_TYPE ra1 = STest::Pick::lowerUpper(0, 0xffffffff);
        NATIVE_INT_TYPE ra2 = STest::Pick::lowerUpper(0, 0xffffffff);
        NATIVE_INT_TYPE ra3 = STest::Pick::lowerUpper(0, 0xffffffff);
        NATIVE_INT_TYPE ra4 = STest::Pick::lowerUpper(0, 0xffffffff);
        NATIVE_INT_TYPE ra5 = STest::Pick::lowerUpper(0, 0xffffffff);
        NATIVE_INT_TYPE ra6 = STest::Pick::lowerUpper(0, 0xffffffff);
        switch (random) {
            case 0:
                Fw::Logger::logMsg("No args");
                truth.check("No args");
                break;
            case 1:
                Fw::Logger::logMsg("One arg: %lu", ra1);
                truth.check("One arg: %lu", ra1);
                break;
            case 2:
                Fw::Logger::logMsg("Two arg: %lu", ra1, ra2);
                truth.check("Two arg: %lu", ra1, ra2);
                break;
            case 3:
                Fw::Logger::logMsg("Three arg: %lu", ra1, ra2, ra3);
                truth.check("Three arg: %lu", ra1, ra2, ra3);
                break;
            case 4:
                Fw::Logger::logMsg("Four arg: %lu", ra1, ra2, ra3, ra4);
                truth.check("Four arg: %lu", ra1, ra2, ra3, ra4);
                break;
            case 5:
                Fw::Logger::logMsg("Five arg: %lu", ra1, ra2, ra3, ra4, ra5);
                truth.check("Five arg: %lu", ra1, ra2, ra3, ra4, ra5);
                break;
            case 6:
                Fw::Logger::logMsg("Six arg: %lu", ra1, ra2, ra3, ra4, ra5, ra6);
                truth.check("Six arg: %lu", ra1, ra2, ra3, ra4, ra5, ra6);
                break;
            default:
                ASSERT_EQ(0, 1);
        }
        truth.reset();
    }

    // Constructor
    LogBad::LogBad(const Fw::EightyCharString& name) : STest::Rule<MockLogging::FakeLogger>(name.m_buf) {}

    // Check for logging, only when not NULL
    bool LogBad::precondition(const MockLogging::FakeLogger& truth) {
        return truth.s_current == NULL;
    }

    // Log valid messages
    void LogBad::action(MockLogging::FakeLogger& truth) {
        NATIVE_INT_TYPE random = STest::Pick::lowerUpper(0, 6);
        NATIVE_INT_TYPE ra1 = STest::Pick::lowerUpper(0, 0xffffffff);
        NATIVE_INT_TYPE ra2 = STest::Pick::lowerUpper(0, 0xffffffff);
        NATIVE_INT_TYPE ra3 = STest::Pick::lowerUpper(0, 0xffffffff);
        NATIVE_INT_TYPE ra4 = STest::Pick::lowerUpper(0, 0xffffffff);
        NATIVE_INT_TYPE ra5 = STest::Pick::lowerUpper(0, 0xffffffff);
        NATIVE_INT_TYPE ra6 = STest::Pick::lowerUpper(0, 0xffffffff);
        switch (random) {
            case 0:
                Fw::Logger::logMsg("No args");
                truth.check(NULL);
                break;
            case 1:
                Fw::Logger::logMsg("One arg: %lu", ra1);
                truth.check(NULL);
                break;
            case 2:
                Fw::Logger::logMsg("Two arg: %lu", ra1, ra2);
                truth.check(NULL);
                break;
            case 3:
                Fw::Logger::logMsg("Three arg: %lu", ra1, ra2, ra3);
                truth.check(NULL);
                break;
            case 4:
                Fw::Logger::logMsg("Four arg: %lu", ra1, ra2, ra3, ra4);
                truth.check(NULL);
                break;
            case 5:
                Fw::Logger::logMsg("Five arg: %lu", ra1, ra2, ra3, ra4, ra5);
                truth.check(NULL);
                break;
            case 6:
                Fw::Logger::logMsg("Six arg: %lu", ra1, ra2, ra3, ra4, ra5, ra6);
                truth.check(NULL);
                break;
            default:
                ASSERT_EQ(0, 1);
        }
        truth.reset();
    }
};
