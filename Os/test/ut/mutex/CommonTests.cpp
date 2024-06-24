// ======================================================================
// \title Os/test/ut/mutex/CommonTests.cpp
// \brief common test implementations
// ======================================================================
#include "Os/test/ut/mutex/CommonTests.hpp"
#include <gtest/gtest.h>
#include "Os/Mutex.hpp"


// Ensure that close mode changes work reliably
// TEST_F(Functionality, Close) {
//     Os::Test::File::Tester::OpenFileCreate create_rule(false);
//     Os::Test::File::Tester::CloseFile close_rule;
//     create_rule.apply(*tester);
//     close_rule.apply(*tester);
// }

// Ensure that the assignment operator works correctly
// TEST_F(Functionality, AssignmentOperator) {
//     Os::Test::File::Tester::OpenFileCreate open_rule(false);
//     Os::Test::File::Tester::CopyAssignment copy_rule;
//     Os::Test::File::Tester::CloseFile close_rule;
//     open_rule.apply(*tester);
//     copy_rule.apply(*tester);
//     close_rule.apply(*tester);
// }

// Randomized testing on the interfaces
// TEST_F(Functionality, RandomizedInterfaceTesting) {
//     // Enumerate all rules and construct an instance of each
//     Os::Test::File::Tester::OpenFileCreateOverwrite open_file_create_overwrite_rule(true);
//     Os::Test::File::Tester::CloseFile close_file_rule;
//     Os::Test::File::Tester::CopyConstruction copy_construction;
//     Os::Test::File::Tester::CopyAssignment copy_assignment;
//     Os::Test::File::Tester::OpenInvalidModes open_invalid_modes_rule;
//     Os::Test::File::Tester::PreallocateWithoutOpen preallocate_without_open_rule;
//     Os::Test::File::Tester::SeekWithoutOpen seek_without_open_rule;
//     Os::Test::File::Tester::FlushInvalidModes flush_invalid_modes_rule;
//     Os::Test::File::Tester::ReadInvalidModes read_invalid_modes_rule;
//     Os::Test::File::Tester::WriteInvalidModes write_invalid_modes_rule;
//     Os::Test::File::Tester::OpenIllegalPath open_illegal_path;
//     Os::Test::File::Tester::OpenIllegalMode open_illegal_mode;
//     Os::Test::File::Tester::PreallocateIllegalOffset preallocate_illegal_offset;
//     Os::Test::File::Tester::PreallocateIllegalLength preallocate_illegal_length;
//     Os::Test::File::Tester::SeekIllegal seek_illegal;
//     Os::Test::File::Tester::ReadIllegalBuffer read_illegal_buffer;
//     Os::Test::File::Tester::ReadIllegalSize read_illegal_size;
//     Os::Test::File::Tester::WriteIllegalBuffer write_illegal_buffer;
//     Os::Test::File::Tester::WriteIllegalSize write_illegal_size;
//     Os::Test::File::Tester::IncrementalCrcInvalidModes incremental_invalid_mode_rule;
//     Os::Test::File::Tester::FullCrcInvalidModes full_invalid_mode_rule;

//     // Place these rules into a list of rules
//     STest::Rule<Os::Test::File::Tester>* rules[] = {
//             &open_file_create_overwrite_rule,
//             &close_file_rule,
//             &copy_assignment,
//             &copy_construction,
//             &open_invalid_modes_rule,
//             &preallocate_without_open_rule,
//             &seek_without_open_rule,
//             &flush_invalid_modes_rule,
//             &read_invalid_modes_rule,
//             &write_invalid_modes_rule,
//             &open_illegal_path,
//             &open_illegal_mode,
//             &preallocate_illegal_offset,
//             &preallocate_illegal_length,
//             &seek_illegal,
//             &read_illegal_buffer,
//             &read_illegal_size,
//             &write_illegal_buffer,
//             &write_illegal_size,
//             &incremental_invalid_mode_rule,
//             &full_invalid_mode_rule
//     };

//     // Take the rules and place them into a random scenario
//     STest::RandomScenario<Os::Test::File::Tester> random(
//             "Random Rules",
//             rules,
//             FW_NUM_ARRAY_ELEMENTS(rules)
//     );

//     // Create a bounded scenario wrapping the random scenario
//     STest::BoundedScenario<Os::Test::File::Tester> bounded(
//             "Bounded Random Rules Scenario",
//             random,
//             RANDOM_BOUND/10
//     );
//     // Run!
//     const U32 numSteps = bounded.run(*tester);
//     printf("Ran %u steps.\n", numSteps);
// }

