#include <gtest/gtest.h>
#include <Svc/Types/EventSeverityFilter/EventSeverityFilter.hpp>

namespace Svc {

TEST(EventSeverityFilter, DefaultState) {
    EventSeverityFilter filter;

    // All severity levels should be enabled by default
    ASSERT_TRUE(filter.isEnabled(Fw::LogSeverity::WARNING_HI));
    ASSERT_TRUE(filter.isEnabled(Fw::LogSeverity::WARNING_LO));
    ASSERT_TRUE(filter.isEnabled(Fw::LogSeverity::COMMAND));
    ASSERT_TRUE(filter.isEnabled(Fw::LogSeverity::ACTIVITY_HI));
    ASSERT_TRUE(filter.isEnabled(Fw::LogSeverity::ACTIVITY_LO));
    ASSERT_TRUE(filter.isEnabled(Fw::LogSeverity::DIAGNOSTIC));
    ASSERT_TRUE(filter.isEnabled(Fw::LogSeverity::FATAL));

    // Nothing should be filtered in default state
    ASSERT_FALSE(filter.isFiltered(Fw::LogSeverity::WARNING_HI));
    ASSERT_FALSE(filter.isFiltered(Fw::LogSeverity::WARNING_LO));
    ASSERT_FALSE(filter.isFiltered(Fw::LogSeverity::COMMAND));
    ASSERT_FALSE(filter.isFiltered(Fw::LogSeverity::ACTIVITY_HI));
    ASSERT_FALSE(filter.isFiltered(Fw::LogSeverity::ACTIVITY_LO));
    ASSERT_FALSE(filter.isFiltered(Fw::LogSeverity::DIAGNOSTIC));
    ASSERT_FALSE(filter.isFiltered(Fw::LogSeverity::FATAL));
}

TEST(EventSeverityFilter, DisableEachSeverity) {
    // Verify each severity level can be individually disabled
    const Fw::LogSeverity::t severities[] = {
        Fw::LogSeverity::WARNING_HI,  Fw::LogSeverity::WARNING_LO,  Fw::LogSeverity::COMMAND,
        Fw::LogSeverity::ACTIVITY_HI, Fw::LogSeverity::ACTIVITY_LO, Fw::LogSeverity::DIAGNOSTIC,
    };

    for (const auto sev : severities) {
        EventSeverityFilter filter;
        filter.setFilter(sev, false);

        // Only the disabled severity should be filtered
        ASSERT_TRUE(filter.isFiltered(sev));
        ASSERT_FALSE(filter.isEnabled(sev));

        // All other severities should still pass
        for (const auto otherSev : severities) {
            if (otherSev != sev) {
                ASSERT_FALSE(filter.isFiltered(otherSev));
                ASSERT_TRUE(filter.isEnabled(otherSev));
            }
        }
    }
}

TEST(EventSeverityFilter, FatalNeverFiltered) {
    EventSeverityFilter filter;

    // Disable all non-FATAL severities
    filter.setFilter(Fw::LogSeverity::WARNING_HI, false);
    filter.setFilter(Fw::LogSeverity::WARNING_LO, false);
    filter.setFilter(Fw::LogSeverity::COMMAND, false);
    filter.setFilter(Fw::LogSeverity::ACTIVITY_HI, false);
    filter.setFilter(Fw::LogSeverity::ACTIVITY_LO, false);
    filter.setFilter(Fw::LogSeverity::DIAGNOSTIC, false);

    // FATAL should still pass through
    ASSERT_FALSE(filter.isFiltered(Fw::LogSeverity::FATAL));
    ASSERT_TRUE(filter.isEnabled(Fw::LogSeverity::FATAL));

    // Attempting to disable FATAL should have no effect
    filter.setFilter(Fw::LogSeverity::FATAL, false);
    ASSERT_FALSE(filter.isFiltered(Fw::LogSeverity::FATAL));
    ASSERT_TRUE(filter.isEnabled(Fw::LogSeverity::FATAL));
}

TEST(EventSeverityFilter, EnableAfterDisable) {
    EventSeverityFilter filter;

    // Disable DIAGNOSTIC
    filter.setFilter(Fw::LogSeverity::DIAGNOSTIC, false);
    ASSERT_TRUE(filter.isFiltered(Fw::LogSeverity::DIAGNOSTIC));

    // Re-enable DIAGNOSTIC
    filter.setFilter(Fw::LogSeverity::DIAGNOSTIC, true);
    ASSERT_FALSE(filter.isFiltered(Fw::LogSeverity::DIAGNOSTIC));
    ASSERT_TRUE(filter.isEnabled(Fw::LogSeverity::DIAGNOSTIC));
}

TEST(EventSeverityFilter, DisableAllNonFatal) {
    EventSeverityFilter filter;

    filter.setFilter(Fw::LogSeverity::WARNING_HI, false);
    filter.setFilter(Fw::LogSeverity::WARNING_LO, false);
    filter.setFilter(Fw::LogSeverity::COMMAND, false);
    filter.setFilter(Fw::LogSeverity::ACTIVITY_HI, false);
    filter.setFilter(Fw::LogSeverity::ACTIVITY_LO, false);
    filter.setFilter(Fw::LogSeverity::DIAGNOSTIC, false);

    // All non-FATAL should be filtered
    ASSERT_TRUE(filter.isFiltered(Fw::LogSeverity::WARNING_HI));
    ASSERT_TRUE(filter.isFiltered(Fw::LogSeverity::WARNING_LO));
    ASSERT_TRUE(filter.isFiltered(Fw::LogSeverity::COMMAND));
    ASSERT_TRUE(filter.isFiltered(Fw::LogSeverity::ACTIVITY_HI));
    ASSERT_TRUE(filter.isFiltered(Fw::LogSeverity::ACTIVITY_LO));
    ASSERT_TRUE(filter.isFiltered(Fw::LogSeverity::DIAGNOSTIC));

    // FATAL should still pass
    ASSERT_FALSE(filter.isFiltered(Fw::LogSeverity::FATAL));
}

}  // namespace Svc

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
