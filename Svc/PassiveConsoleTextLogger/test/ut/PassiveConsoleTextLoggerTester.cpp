#include "PassiveConsoleTextLoggerTester.hpp"
#include <Fw/Logger/Logger.hpp>
#include <cstring>

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10

namespace Svc {

// A test logger that captures messages
class TestLogger : public Fw::Logger {
  public:
    TestLogger() : m_count(0) { memset(m_messages, 0, sizeof(m_messages)); }

    void writeMessage(const Fw::ConstStringBase& message) override {
        if (m_count < MAX_MESSAGES) {
            strncpy(m_messages[m_count], message.toChar(), MAX_MSG_LEN - 1);
            m_messages[m_count][MAX_MSG_LEN - 1] = '\0';
            m_count++;
        }
    }

    void reset() {
        m_count = 0;
        memset(m_messages, 0, sizeof(m_messages));
    }

    static constexpr FwSizeType MAX_MESSAGES = 32;
    static constexpr FwSizeType MAX_MSG_LEN = 512;
    FwSizeType m_count;
    char m_messages[MAX_MESSAGES][MAX_MSG_LEN];
};

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

PassiveConsoleTextLoggerTester::PassiveConsoleTextLoggerTester()
    : PassiveTextLoggerGTestBase("Tester", MAX_HISTORY_SIZE), component("PassiveConsoleTextLogger") {
    this->initComponents();
    this->connectPorts();
}

PassiveConsoleTextLoggerTester::~PassiveConsoleTextLoggerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void PassiveConsoleTextLoggerTester::testSeverityFilter() {
    TestLogger logger;
    Fw::Logger::registerLogger(&logger);

    FwEventIdType id = 1;
    Fw::Time timeTag(TimeBase::TB_NONE, 1, 0);

    // Disable WARNING_LO severity
    this->component.setSeverityFilter(Fw::LogSeverity::WARNING_LO, false);

    // Send WARNING_LO event (should be filtered)
    Fw::TextLogString text("This should be filtered");
    this->invoke_to_TextLogger(0, id, timeTag, Fw::LogSeverity::WARNING_LO, text);

    // Verify nothing was logged
    ASSERT_EQ(0U, logger.m_count);

    // Send WARNING_HI event (should pass through)
    id = 2;
    Fw::TextLogString text2("This should pass through");
    this->invoke_to_TextLogger(0, id, timeTag, Fw::LogSeverity::WARNING_HI, text2);

    // Verify one message was logged with WARNING_HI content
    ASSERT_EQ(1U, logger.m_count);
    ASSERT_NE(nullptr, strstr(logger.m_messages[0], "WARNING_HI"));
    ASSERT_NE(nullptr, strstr(logger.m_messages[0], "This should pass through"));

    // Re-enable WARNING_LO and verify it passes
    logger.reset();
    this->component.setSeverityFilter(Fw::LogSeverity::WARNING_LO, true);

    id = 3;
    Fw::TextLogString text3("Now passes");
    this->invoke_to_TextLogger(0, id, timeTag, Fw::LogSeverity::WARNING_LO, text3);
    ASSERT_EQ(1U, logger.m_count);
    ASSERT_NE(nullptr, strstr(logger.m_messages[0], "WARNING_LO"));

    // Unregister test logger
    Fw::Logger::registerLogger(nullptr);
}

void PassiveConsoleTextLoggerTester::testSeverityFilterFatalNeverFiltered() {
    TestLogger logger;
    Fw::Logger::registerLogger(&logger);

    FwEventIdType id = 1;
    Fw::Time timeTag(TimeBase::TB_NONE, 1, 0);

    // Disable all filterable severities
    this->component.setSeverityFilter(Fw::LogSeverity::WARNING_HI, false);
    this->component.setSeverityFilter(Fw::LogSeverity::WARNING_LO, false);
    this->component.setSeverityFilter(Fw::LogSeverity::COMMAND, false);
    this->component.setSeverityFilter(Fw::LogSeverity::ACTIVITY_HI, false);
    this->component.setSeverityFilter(Fw::LogSeverity::ACTIVITY_LO, false);
    this->component.setSeverityFilter(Fw::LogSeverity::DIAGNOSTIC, false);

    // FATAL should still pass through
    Fw::TextLogString text("Fatal event must pass");
    this->invoke_to_TextLogger(0, id, timeTag, Fw::LogSeverity::FATAL, text);

    ASSERT_EQ(1U, logger.m_count);
    ASSERT_NE(nullptr, strstr(logger.m_messages[0], "FATAL"));
    ASSERT_NE(nullptr, strstr(logger.m_messages[0], "Fatal event must pass"));

    // Non-fatal should be filtered
    logger.reset();
    id = 2;
    Fw::TextLogString text2("Should be dropped");
    this->invoke_to_TextLogger(0, id, timeTag, Fw::LogSeverity::ACTIVITY_HI, text2);
    ASSERT_EQ(0U, logger.m_count);

    // Unregister test logger
    Fw::Logger::registerLogger(nullptr);
}

void PassiveConsoleTextLoggerTester::testSeverityFilterDisableAll() {
    TestLogger logger;
    Fw::Logger::registerLogger(&logger);

    FwEventIdType id = 1;
    Fw::Time timeTag(TimeBase::TB_NONE, 1, 0);

    // Disable all filterable severities
    this->component.setSeverityFilter(Fw::LogSeverity::WARNING_HI, false);
    this->component.setSeverityFilter(Fw::LogSeverity::WARNING_LO, false);
    this->component.setSeverityFilter(Fw::LogSeverity::COMMAND, false);
    this->component.setSeverityFilter(Fw::LogSeverity::ACTIVITY_HI, false);
    this->component.setSeverityFilter(Fw::LogSeverity::ACTIVITY_LO, false);
    this->component.setSeverityFilter(Fw::LogSeverity::DIAGNOSTIC, false);

    // Send one of each filterable severity - all should be dropped
    Fw::TextLogString text("Dropped");
    this->invoke_to_TextLogger(0, id++, timeTag, Fw::LogSeverity::WARNING_HI, text);
    this->invoke_to_TextLogger(0, id++, timeTag, Fw::LogSeverity::WARNING_LO, text);
    this->invoke_to_TextLogger(0, id++, timeTag, Fw::LogSeverity::COMMAND, text);
    this->invoke_to_TextLogger(0, id++, timeTag, Fw::LogSeverity::ACTIVITY_HI, text);
    this->invoke_to_TextLogger(0, id++, timeTag, Fw::LogSeverity::ACTIVITY_LO, text);
    this->invoke_to_TextLogger(0, id++, timeTag, Fw::LogSeverity::DIAGNOSTIC, text);

    ASSERT_EQ(0U, logger.m_count);

    // Unregister test logger
    Fw::Logger::registerLogger(nullptr);
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void PassiveConsoleTextLoggerTester::connectPorts() {
    this->connect_to_TextLogger(0, this->component.get_TextLogger_InputPort(0));
}

void PassiveConsoleTextLoggerTester::initComponents() {
    this->init(INSTANCE);
    this->component.init(INSTANCE);
}

}  // namespace Svc
