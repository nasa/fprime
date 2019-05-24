/**
 * LoggerRules.hpp:
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
#ifndef FPRIME_LOGGERRULES_HPP
#define FPRIME_LOGGERRULES_HPP

#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/EightyCharString.hpp>
#include <Fw/Logger/test/ut/FakeLogger.hpp>
#include <STest/STest/Rule/Rule.hpp>
#include <STest/STest/Pick/Pick.hpp>


namespace LoggerRules {

    /**
     * Register:
     *
     * Rule to handle the registration of a logger to the global logger. It may also register a "NULL" logger and thus
     * stop output logging.
     */
    struct Register : public STest::Rule<MockLogging::FakeLogger> {
        // Constructor
        Register(const Fw::EightyCharString& name) : STest::Rule<MockLogging::FakeLogger>(name.m_buf) {}

        // Check for registration, always allowed
        bool precondition(const MockLogging::FakeLogger& truth) {
            return true;
        }

        // Register NULL or truth as the system logger
        void action(MockLogging::FakeLogger& truth) {
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
    };
    /**
     * LogGood:
     *
     * As long as a non-NULL logger is set as the system logger, then valid log messages should be processed.
     */
    struct LogGood : public STest::Rule<MockLogging::FakeLogger> {
        // Constructor
        LogGood(const Fw::EightyCharString& name) : STest::Rule<MockLogging::FakeLogger>(name.m_buf) {}

        // Check for logging, only when not NULL
        bool precondition(const MockLogging::FakeLogger& truth) {
            return truth.s_current != NULL;
        }

        // Log valid messages
        void action(MockLogging::FakeLogger& truth) {
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
    };

    /**
     * LogBad:
     *
     * As long as a non-NULL logger is set as the system logger, then valid log messages should be processed.
     */
    struct LogBad : public STest::Rule<MockLogging::FakeLogger> {
        // Constructor
        LogBad(const Fw::EightyCharString& name) : STest::Rule<MockLogging::FakeLogger>(name.m_buf) {}

        // Check for logging, only when not NULL
        bool precondition(const MockLogging::FakeLogger& truth) {
            return truth.s_current == NULL;
        }

        // Log valid messages
        void action(MockLogging::FakeLogger& truth) {
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
};
#endif //FPRIME_LOGGERRULES_HPP
