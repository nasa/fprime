// ======================================================================
// \title Os/test/ut/rawtime/CommonTests.cpp
// \brief common test implementations
// ======================================================================
#include "Os/test/ut/rawtime/CommonTests.hpp"
#include "Fw/Buffer/Buffer.hpp"

// ----------------------------------------------------------------------
// Test Fixture
// ----------------------------------------------------------------------

std::unique_ptr<Os::Test::RawTime::Tester> get_tester_implementation() {
    return std::unique_ptr<Os::Test::RawTime::Tester>(new Os::Test::RawTime::Tester());
}

Functionality::Functionality() : tester(get_tester_implementation()) {

    tester->m_times.reserve(tester->TEST_TIME_COUNT);
    tester->m_shadow_times.reserve(tester->TEST_TIME_COUNT);

    for (U32 i = 0; i < tester->TEST_TIME_COUNT; ++i) {
        tester->m_times.emplace_back();
        tester->m_shadow_times.emplace_back();
        tester->m_times[i].now();
        tester->m_shadow_times[i] = std::chrono::system_clock::now();
    }
}

void Functionality::SetUp() {
    // All setup is done in the constructor (recommended by GTest)
}

void Functionality::TearDown() {
    // No teardown required
}

// ----------------------------------------------------------------------
// Test Cases
// ----------------------------------------------------------------------

// Now
TEST_F(Functionality, Now) {
    Os::Test::RawTime::Tester::Now get_time_rule;
    get_time_rule.apply(*tester);
}

// SelfDiffIsZero
TEST_F(Functionality, SelfDiffIsZero) {
    Os::Test::RawTime::Tester::SelfDiffIsZero self_diff_rule;
    self_diff_rule.apply(*tester);
}

// GetTimeDiffU32
TEST_F(Functionality, GetTimeDiffU32) {
    Os::Test::RawTime::Tester::GetTimeDiffU32 get_diff_rule;
    get_diff_rule.apply(*tester);
}

// GetTimeInterval
TEST_F(Functionality, GetTimeInterval) {
    Os::Test::RawTime::Tester::GetTimeInterval get_interval_rule;
    get_interval_rule.apply(*tester);
}

// Serialization
TEST_F(Functionality, Serialization) {
    Os::Test::RawTime::Tester::Serialization serialization_rule;
    serialization_rule.apply(*tester);
}

// DiffU32 overflows if times are too far apart
TEST_F(Functionality, DiffU32Overflow) {
    Os::Test::RawTime::Tester::DiffU32Overflow overflow_rule;
    overflow_rule.apply(*tester);
}

// RandomizedTesting
TEST_F(Functionality, RandomizedTesting) {
    // Enumerate all rules and construct an instance of each
    Os::Test::RawTime::Tester::Now get_time_rule;
    Os::Test::RawTime::Tester::SelfDiffIsZero diff_zero_rule;
    Os::Test::RawTime::Tester::GetTimeDiffU32 get_diff_rule;
    Os::Test::RawTime::Tester::GetTimeInterval get_interval_rule;
    Os::Test::RawTime::Tester::Serialization serialization_rule;
    Os::Test::RawTime::Tester::DiffU32Overflow overflow_rule;

    // Place these rules into a list of rules
    STest::Rule<Os::Test::RawTime::Tester>* rules[] = {
            &get_time_rule,
            &diff_zero_rule,
            &get_diff_rule,
            &get_interval_rule,
            &serialization_rule,
            &overflow_rule,
    };

    // Take the rules and place them into a random scenario
    STest::RandomScenario<Os::Test::RawTime::Tester> random(
            "Random Rules",
            rules,
            FW_NUM_ARRAY_ELEMENTS(rules)
    );

    // Create a bounded scenario wrapping the random scenario
    STest::BoundedScenario<Os::Test::RawTime::Tester> bounded(
            "Bounded Random Rules Scenario",
            random,
            5000
    );
    // Run!
    const U32 numSteps = bounded.run(*tester);
    printf("Ran %u steps for RawTime.\n", numSteps);

}

