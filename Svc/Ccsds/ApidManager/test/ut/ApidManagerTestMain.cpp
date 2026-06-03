// ======================================================================
// \title  ApidManagerTestMain.cpp
// \author thomas-bc
// \brief  cpp file for ApidManager component test main function
// ======================================================================

#include "STest/Random/Random.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "Svc/Ccsds/ApidManager/test/ut/ApidManagerTester.hpp"

using Svc::Ccsds::ApidManagerTester;

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

// Verify that getApidSeqCountIn registers a new APID and returns
// incrementing counts on subsequent calls.
TEST(ApidManager, GetSequenceCounts) {
    ApidManagerTester tester;
    ApidManagerTester::GetSeqCount__NewOk ruleNewOk;
    ApidManagerTester::GetSeqCount__Existing ruleExisting;
    ruleNewOk.apply(tester);     // register a new APID; expect count 0
    ruleExisting.apply(tester);  // retrieve count for the same APID; expect count 1
}

// Verify that validateApidSeqCountIn fires no event on a matching count
// and fires UnexpectedSequenceCount on a mismatch.
TEST(ApidManager, ValidateSequenceCounts) {
    ApidManagerTester tester;
    ApidManagerTester::GetSeqCount__NewOk ruleNewOk;
    ApidManagerTester::ValidateSeqCount__Ok ruleValidateOk;
    ApidManagerTester::ValidateSeqCount__Failure ruleValidateFailure;
    ruleNewOk.apply(tester);            // register an APID so validate rules can fire
    ruleValidateOk.apply(tester);       // validate correct count; no event expected
    ruleValidateFailure.apply(tester);  // validate wrong count; event expected
}

// Fill the APID table, then validate a count for an untracked APID.
// Verifies that the component logs ApidTableFull, does NOT log
// UnexpectedSequenceCount, and does not trip an FW_ASSERT trying to
// re-sync the (untracked) APID.
TEST(ApidManager, ValidateSequenceCountWhenTableFull) {
    ApidManagerTester tester;
    ApidManagerTester::GetSeqCount__NewOk ruleGetNewOk;
    ApidManagerTester::ValidateSeqCount__NewTableFull ruleValidateNewTableFull;
    // Apply the NewOk rule until the table is full and the shadow flag flips.
    // The rule itself is responsible for flipping shadow_isTableFull when size has reached MAX.
    FwIndexType iter_bound = 1000;
    while (!tester.shadow.shadow_isTableFull && iter_bound-- > 0) {
        ruleGetNewOk.apply(tester);
    }
    // Now exercise the test path: validate a count for an untracked APID against a full table
    ruleValidateNewTableFull.apply(tester);
}

// Randomized test: apply rules in a random sequence for a large number of iterations
TEST(ApidManager, RandomizedTesting) {
    U32 numRulesToApply = 10000;
    ApidManagerTester tester;
    ApidManagerTester::GetSeqCount__Existing ruleGetExisting;
    ApidManagerTester::GetSeqCount__NewOk ruleGetNewOk;
    ApidManagerTester::GetSeqCount__NewTableFull ruleGetNewTableFull;
    ApidManagerTester::ValidateSeqCount__Ok ruleValidateOk;
    ApidManagerTester::ValidateSeqCount__Failure ruleValidateFailure;
    ApidManagerTester::ValidateSeqCount__NewTableFull ruleValidateNewTableFull;

    STest::Rule<ApidManagerTester>* rules[] = {
        &ruleGetExisting, &ruleGetNewOk,        &ruleGetNewTableFull,
        &ruleValidateOk,  &ruleValidateFailure, &ruleValidateNewTableFull,
    };

    STest::RandomScenario<ApidManagerTester> random("Random Rules", rules, FW_NUM_ARRAY_ELEMENTS(rules));
    STest::BoundedScenario<ApidManagerTester> bounded("Bounded Random Rules Scenario", random, numRulesToApply);
    const U32 numSteps = bounded.run(tester);
    printf("Ran %u steps.\n", numSteps);
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
