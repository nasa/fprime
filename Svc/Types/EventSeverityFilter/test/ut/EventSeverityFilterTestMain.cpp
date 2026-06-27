#include <gtest/gtest.h>
#include <Svc/Types/EventSeverityFilter/EventSeverityFilter.hpp>

namespace Svc {

TEST(EventSeverityFilter, DefaultState) {
    EventSeverityFilter filter;

    // All severity levels should be enabled by default
    ASSERT_EQ(filter.isEnabled(Fw::LogSeverity::WARNING_HI), Fw::Enabled::ENABLED);
    ASSERT_EQ(filter.isEnabled(Fw::LogSeverity::WARNING_LO), Fw::Enabled::ENABLED);
    ASSERT_EQ(filter.isEnabled(Fw::LogSeverity::COMMAND), Fw::Enabled::ENABLED);
    ASSERT_EQ(filter.isEnabled(Fw::LogSeverity::ACTIVITY_HI), Fw::Enabled::ENABLED);
    ASSERT_EQ(filter.isEnabled(Fw::LogSeverity::ACTIVITY_LO), Fw::Enabled::ENABLED);
    ASSERT_EQ(filter.isEnabled(Fw::LogSeverity::DIAGNOSTIC), Fw::Enabled::ENABLED);
    ASSERT_EQ(filter.isEnabled(Fw::LogSeverity::FATAL), Fw::Enabled::ENABLED);

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
    const Fw::LogSeverity::t severities[] = {
        Fw::LogSeverity::WARNING_HI,  Fw::LogSeverity::WARNING_LO,  Fw::LogSeverity::COMMAND,
        Fw::LogSeverity::ACTIVITY_HI, Fw::LogSeverity::ACTIVITY_LO, Fw::LogSeverity::DIAGNOSTIC,
    };

    for (const auto sev : severities) {
        EventSeverityFilter filter;
        filter.setFilter(sev, Fw::Enabled::DISABLED);

        ASSERT_TRUE(filter.isFiltered(sev));
        ASSERT_EQ(filter.isEnabled(sev), Fw::Enabled::DISABLED);

        for (const auto otherSev : severities) {
            if (otherSev != sev) {
                ASSERT_FALSE(filter.isFiltered(otherSev));
                ASSERT_EQ(filter.isEnabled(otherSev), Fw::Enabled::ENABLED);
            }
        }
    }
}

TEST(EventSeverityFilter, FatalNeverFiltered) {
    EventSeverityFilter filter;

    filter.setFilter(Fw::LogSeverity::WARNING_HI, Fw::Enabled::DISABLED);
    filter.setFilter(Fw::LogSeverity::WARNING_LO, Fw::Enabled::DISABLED);
    filter.setFilter(Fw::LogSeverity::COMMAND, Fw::Enabled::DISABLED);
    filter.setFilter(Fw::LogSeverity::ACTIVITY_HI, Fw::Enabled::DISABLED);
    filter.setFilter(Fw::LogSeverity::ACTIVITY_LO, Fw::Enabled::DISABLED);
    filter.setFilter(Fw::LogSeverity::DIAGNOSTIC, Fw::Enabled::DISABLED);

    ASSERT_FALSE(filter.isFiltered(Fw::LogSeverity::FATAL));
    ASSERT_EQ(filter.isEnabled(Fw::LogSeverity::FATAL), Fw::Enabled::ENABLED);

    // Attempting to disable FATAL should have no effect
    filter.setFilter(Fw::LogSeverity::FATAL, Fw::Enabled::DISABLED);
    ASSERT_FALSE(filter.isFiltered(Fw::LogSeverity::FATAL));
    ASSERT_EQ(filter.isEnabled(Fw::LogSeverity::FATAL), Fw::Enabled::ENABLED);
}

TEST(EventSeverityFilter, EnableAfterDisable) {
    EventSeverityFilter filter;

    filter.setFilter(Fw::LogSeverity::DIAGNOSTIC, Fw::Enabled::DISABLED);
    ASSERT_TRUE(filter.isFiltered(Fw::LogSeverity::DIAGNOSTIC));

    filter.setFilter(Fw::LogSeverity::DIAGNOSTIC, Fw::Enabled::ENABLED);
    ASSERT_FALSE(filter.isFiltered(Fw::LogSeverity::DIAGNOSTIC));
    ASSERT_EQ(filter.isEnabled(Fw::LogSeverity::DIAGNOSTIC), Fw::Enabled::ENABLED);
}

TEST(EventSeverityFilter, DisableAllNonFatal) {
    EventSeverityFilter filter;

    filter.setFilter(Fw::LogSeverity::WARNING_HI, Fw::Enabled::DISABLED);
    filter.setFilter(Fw::LogSeverity::WARNING_LO, Fw::Enabled::DISABLED);
    filter.setFilter(Fw::LogSeverity::COMMAND, Fw::Enabled::DISABLED);
    filter.setFilter(Fw::LogSeverity::ACTIVITY_HI, Fw::Enabled::DISABLED);
    filter.setFilter(Fw::LogSeverity::ACTIVITY_LO, Fw::Enabled::DISABLED);
    filter.setFilter(Fw::LogSeverity::DIAGNOSTIC, Fw::Enabled::DISABLED);

    ASSERT_TRUE(filter.isFiltered(Fw::LogSeverity::WARNING_HI));
    ASSERT_TRUE(filter.isFiltered(Fw::LogSeverity::WARNING_LO));
    ASSERT_TRUE(filter.isFiltered(Fw::LogSeverity::COMMAND));
    ASSERT_TRUE(filter.isFiltered(Fw::LogSeverity::ACTIVITY_HI));
    ASSERT_TRUE(filter.isFiltered(Fw::LogSeverity::ACTIVITY_LO));
    ASSERT_TRUE(filter.isFiltered(Fw::LogSeverity::DIAGNOSTIC));

    ASSERT_FALSE(filter.isFiltered(Fw::LogSeverity::FATAL));
}

}  // namespace Svc

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
