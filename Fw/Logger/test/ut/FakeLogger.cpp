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

    void FakeLogger::log(
            const char *fmt,
            POINTER_CAST a0,
            POINTER_CAST a1,
            POINTER_CAST a2,
            POINTER_CAST a3,
            POINTER_CAST a4,
            POINTER_CAST a5,
            POINTER_CAST a6,
            POINTER_CAST a7,
            POINTER_CAST a8,
            POINTER_CAST a9
    ) {
        m_last.fmt = fmt;
        m_last.a0 = a0;
        m_last.a1 = a1;
        m_last.a2 = a2;
        m_last.a3 = a3;
        m_last.a4 = a4;
        m_last.a5 = a5;
        m_last.a6 = a6;
        m_last.a7 = a7;
        m_last.a8 = a8;
        m_last.a9 = a9;
    }

    void FakeLogger::check(
            const char *fmt,
            POINTER_CAST a0,
            POINTER_CAST a1,
            POINTER_CAST a2,
            POINTER_CAST a3,
            POINTER_CAST a4,
            POINTER_CAST a5,
            POINTER_CAST a6,
            POINTER_CAST a7,
            POINTER_CAST a8,
            POINTER_CAST a9

    ) {
        ASSERT_EQ(m_last.fmt, fmt);
        ASSERT_EQ(m_last.a0, a0);
        ASSERT_EQ(m_last.a1, a1);
        ASSERT_EQ(m_last.a2, a2);
        ASSERT_EQ(m_last.a3, a3);
        ASSERT_EQ(m_last.a4, a4);
        ASSERT_EQ(m_last.a5, a5);
        ASSERT_EQ(m_last.a6, a6);
        ASSERT_EQ(m_last.a7, a7);
        ASSERT_EQ(m_last.a8, a8);
        ASSERT_EQ(m_last.a9, a9);
    }

    void FakeLogger::reset() {
        m_last.fmt = nullptr;
        m_last.a0 = 0;
        m_last.a1 = 0;
        m_last.a2 = 0;
        m_last.a3 = 0;
        m_last.a4 = 0;
        m_last.a5 = 0;
        m_last.a6 = 0;
        m_last.a7 = 0;
        m_last.a8 = 0;
        m_last.a9 = 0;
    }
}
