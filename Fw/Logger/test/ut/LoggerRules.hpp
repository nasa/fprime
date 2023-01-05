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

#include <FpConfig.hpp>
#include <Fw/Types/String.hpp>
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
        Register(const Fw::String& name);

        // Check for registration, always allowed
        bool precondition(const MockLogging::FakeLogger& truth);

        // Register NULL or truth as the system logger
        void action(MockLogging::FakeLogger& truth);
    };
    /**
     * LogGood:
     *
     * As long as a non-NULL logger is set as the system logger, then valid log messages should be processed.
     */
    struct LogGood : public STest::Rule<MockLogging::FakeLogger> {
        // Constructor
        LogGood(const Fw::String& name);

        // Check for logging, only when not NULL
        bool precondition(const MockLogging::FakeLogger& truth);

        // Log valid messages
        void action(MockLogging::FakeLogger& truth);
    };

    /**
     * LogBad:
     *
     * As long as a non-NULL logger is set as the system logger, then valid log messages should be processed.
     */
    struct LogBad : public STest::Rule<MockLogging::FakeLogger> {
        // Constructor
        LogBad(const Fw::String& name);

        // Check for logging, only when not NULL
        bool precondition(const MockLogging::FakeLogger& truth);

        // Log valid messages
        void action(MockLogging::FakeLogger& truth);
    };
};
#endif //FPRIME_LOGGERRULES_HPP
