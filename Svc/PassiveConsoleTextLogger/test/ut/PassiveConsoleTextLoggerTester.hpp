#ifndef PASSIVE_CONSOLE_TEXT_LOGGER_TESTER_HPP
#define PASSIVE_CONSOLE_TEXT_LOGGER_TESTER_HPP

#include "PassiveTextLoggerGTestBase.hpp"
#include "Svc/PassiveConsoleTextLogger/ConsoleTextLoggerImpl.hpp"

namespace Svc {

class PassiveConsoleTextLoggerTester : public PassiveTextLoggerGTestBase {
  public:
    PassiveConsoleTextLoggerTester();
    ~PassiveConsoleTextLoggerTester();

    // Tests
    void testSeverityFilter();
    void testSeverityFilterFatalNeverFiltered();
    void testSeverityFilterDisableAll();

  private:
    void connectPorts();
    void initComponents();

    ConsoleTextLoggerImpl component;
};

}  // namespace Svc

#endif
