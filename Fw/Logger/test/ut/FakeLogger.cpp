/**
 * FakeLogger.cpp:
 *
 * Setup a fake logger for use with the testing. This allows for the capture of messages from the system and ensure that
 * the proper log messages are coming through as expected.
 *
 * @author mstarch
 */
#include <gtest/gtest.h>
#include <Fw/Logger/test/ut/FakeLogger.hpp>

namespace MockLogging {
Fw::Logger* FakeLogger::s_current = nullptr;

FakeLogger::FakeLogger() {
    memset(&m_last, 0, sizeof(m_last));
}

void FakeLogger::write(const char* message, FwSizeType size) {
    m_last = message;
    ASSERT_EQ(m_last.size(), size);
}

void FakeLogger::check(const char* message) {
    ASSERT_EQ(m_last, std::string(message));
}

void FakeLogger::reset() {
    m_last = "";
}
}  // namespace MockLogging
