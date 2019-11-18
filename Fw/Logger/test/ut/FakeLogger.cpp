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

extern "C" {
    #include <string.h>
}
namespace MockLogging {
    Fw::Logger* FakeLogger::s_current = NULL;

    FakeLogger::FakeLogger() {
        memset(&m_last, 0, sizeof(m_last));
    }

    void FakeLogger::log(
            const char *fmt,
            POINTER_CAST a1,
            POINTER_CAST a2,
            POINTER_CAST a3,
            POINTER_CAST a4,
            POINTER_CAST a5,
            POINTER_CAST a6
    ) {
        m_last.fmt = fmt;
        m_last.a1 = a1;
        m_last.a2 = a2;
        m_last.a3 = a3;
        m_last.a4 = a4;
        m_last.a5 = a5;
        m_last.a6 = a6;
    }

    void FakeLogger::check(
            const char *fmt,
            POINTER_CAST a1,
            POINTER_CAST a2,
            POINTER_CAST a3,
            POINTER_CAST a4,
            POINTER_CAST a5,
            POINTER_CAST a6
    ) {
        ASSERT_EQ(m_last.fmt, fmt);
        ASSERT_EQ(m_last.a1, a1);
        ASSERT_EQ(m_last.a2, a2);
        ASSERT_EQ(m_last.a3, a3);
        ASSERT_EQ(m_last.a4, a4);
        ASSERT_EQ(m_last.a5, a5);
        ASSERT_EQ(m_last.a6, a6);
    }

    void FakeLogger::reset() {
        m_last.fmt = 0;
        m_last.a1 = 0;
        m_last.a2 = 0;
        m_last.a3 = 0;
        m_last.a4 = 0;
        m_last.a5 = 0;
        m_last.a6 = 0;
    }
}