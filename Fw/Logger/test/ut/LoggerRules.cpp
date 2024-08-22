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
    } else {
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
    Fw::String correct;
    switch (random) {
        case 0:
            Fw::Logger::log("No args");
            correct = "No args";
            break;
        case 1:
            Fw::Logger::log("One arg: %lu", ra[0]);
            correct.format("One arg: %lu", ra[0]);
            break;
        case 2:
            Fw::Logger::log("Two arg: %lu  %lu", ra[0], ra[1]);
            correct.format("Two arg: %lu  %lu", ra[0], ra[1]);
            break;
        case 3:
            Fw::Logger::log("Three arg: %lu  %lu  %lu", ra[0], ra[1], ra[2]);
            correct.format("Three arg: %lu  %lu  %lu", ra[0], ra[1], ra[2]);
            break;
        case 4:
            Fw::Logger::log("Four arg: %lu  %lu  %lu  %lu", ra[0], ra[1], ra[2], ra[3]);
            correct.format("Four arg: %lu  %lu  %lu  %lu", ra[0], ra[1], ra[2], ra[3]);
            break;
        case 5:
            Fw::Logger::log("Five arg: %lu  %lu  %lu  %lu  %lu", ra[0], ra[1], ra[2], ra[3], ra[4]);
            correct.format("Five arg: %lu  %lu  %lu  %lu  %lu", ra[0], ra[1], ra[2], ra[3], ra[4]);
            break;
        case 6:
            Fw::Logger::log("Six arg: %lu  %lu  %lu  %lu  %lu  %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5]);
            correct.format("Six arg: %lu  %lu  %lu  %lu  %lu  %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5]);
            break;
        case 7:
            Fw::Logger::log("Seven arg: %lu  %lu  %lu  %lu  %lu  %lu  %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5],
                            ra[6]);
            correct.format("Seven arg: %lu  %lu  %lu  %lu  %lu  %lu  %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5],
                           ra[6]);
            break;
        case 8:
            Fw::Logger::log("Eight arg: %lu  %lu  %lu  %lu  %lu  %lu  %lu  %lu", ra[0], ra[1], ra[2], ra[3], ra[4],
                            ra[5], ra[6], ra[7]);
            correct.format("Eight arg: %lu  %lu  %lu  %lu  %lu  %lu  %lu  %lu", ra[0], ra[1], ra[2], ra[3], ra[4],
                           ra[5], ra[6], ra[7]);
            break;
        case 9:
            Fw::Logger::log("Nine arg: %lu  %lu  %lu  %lu  %lu  %lu  %lu  %lu  %lu", ra[0], ra[1], ra[2], ra[3], ra[4],
                            ra[5], ra[6], ra[7], ra[8]);
            correct.format("Nine arg: %lu  %lu  %lu  %lu  %lu  %lu  %lu  %lu  %lu", ra[0], ra[1], ra[2], ra[3], ra[4],
                           ra[5], ra[6], ra[7], ra[8]);
            break;
        case 10:
            Fw::Logger::log("Ten arg: %lu  %lu  %lu  %lu  %lu  %lu  %lu  %lu  %lu  %lu", ra[0], ra[1], ra[2], ra[3],
                            ra[4], ra[5], ra[6], ra[7], ra[8], ra[9]);
            correct.format("Ten arg: %lu  %lu  %lu  %lu  %lu  %lu  %lu  %lu  %lu  %lu", ra[0], ra[1], ra[2], ra[3],
                           ra[4], ra[5], ra[6], ra[7], ra[8], ra[9]);
            break;

        default:
            ASSERT_EQ(0, 1);
    }
    truth.check(correct.toChar());
    truth.reset();
}

// Constructor
LogGoodStringObject::LogGoodStringObject(const Fw::String& name)
    : STest::Rule<MockLogging::FakeLogger>(name.toChar()) {}

// Check for logging, only when not NULL
bool LogGoodStringObject::precondition(const MockLogging::FakeLogger& truth) {
    return truth.s_current != nullptr;
}

// Log valid messages
void LogGoodStringObject::action(MockLogging::FakeLogger& truth) {
    Fw::String my_string;
    NATIVE_INT_TYPE random = STest::Pick::lowerUpper(0, my_string.getCapacity() - 1);
    for (int i = 0; i < random; ++i) {
        const_cast<char*>(my_string.toChar())[i] =
            static_cast<char>(STest::Pick::lowerUpper(0, std::numeric_limits<unsigned char>::max()));
    }
    const_cast<char*>(my_string.toChar())[random] = 0;
    Fw::String copy1 = my_string.toChar();
    Fw::Logger::log(copy1);
    truth.check(my_string.toChar());
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
            Fw::Logger::log("No args");
            break;
        case 1:
            Fw::Logger::log("One arg: %lu", ra[0]);
            break;
        case 2:
            Fw::Logger::log("Two arg: %lu", ra[0], ra[1]);
            break;
        case 3:
            Fw::Logger::log("Three arg: %lu", ra[0], ra[1], ra[2]);
            break;
        case 4:
            Fw::Logger::log("Four arg: %lu", ra[0], ra[1], ra[2], ra[3]);
            break;
        case 5:
            Fw::Logger::log("Five arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4]);
            break;
        case 6:
            Fw::Logger::log("Six arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5]);
            break;
        case 7:
            Fw::Logger::log("Seven arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5], ra[6]);
            break;
        case 8:
            Fw::Logger::log("Eight arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5], ra[6], ra[7]);
            break;
        case 9:
            Fw::Logger::log("Nine arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5], ra[6], ra[7], ra[8]);
            break;
        case 10:
            Fw::Logger::log("Ten arg: %lu", ra[0], ra[1], ra[2], ra[3], ra[4], ra[5], ra[6], ra[7], ra[8], ra[9]);
            break;
        default:
            ASSERT_EQ(0, 1);
    }
    truth.check("");
    truth.reset();
}
};  // namespace LoggerRules
