#include "PassiveConsoleTextLoggerTester.hpp"

TEST(Filtering, SeverityFilter) {
    Svc::PassiveConsoleTextLoggerTester tester;
    tester.testSeverityFilter();
}

TEST(Filtering, FatalNeverFiltered) {
    Svc::PassiveConsoleTextLoggerTester tester;
    tester.testSeverityFilterFatalNeverFiltered();
}

TEST(Filtering, DisableAll) {
    Svc::PassiveConsoleTextLoggerTester tester;
    tester.testSeverityFilterDisableAll();
}
