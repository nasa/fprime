// ======================================================================
// \title  ApidManagerTestMain.cpp
// \author thomas-bc
// \brief  cpp file for ApidManager component test main function
// ======================================================================

#include "STest/Random/Random.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "Svc/Ccsds/ApidManager/test/ut/ApidManagerTester.hpp"
#include "Svc/Ccsds/ApidManager/test/ut/Rules/Rules.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

// Verify that getApidSeqCountIn registers a new APID and returns
// incrementing counts on subsequent calls.
TEST(ApidManager, GetSequenceCounts) {
    ApidManagerTester state;
    Rules::GetSeqCount::NewOk ruleNewOk;
    Rules::GetSeqCount::Existing ruleExisting;
    ruleNewOk.apply(state);     // register a new APID; expect count 0
    ruleExisting.apply(state);  // retrieve count for the same APID; expect count 1
}

// Verify that validateApidSeqCountIn fires no event on a matching count
// and fires UnexpectedSequenceCount on a mismatch.
TEST(ApidManager, ValidateSequenceCounts) {
    ApidManagerTester state;
    Rules::GetSeqCount::NewOk ruleNewOk;
    Rules::ValidateSeqCount::Ok ruleValidateOk;
    Rules::ValidateSeqCount::Failure ruleValidateFailure;
    ruleNewOk.apply(state);            // register an APID so validate rules can fire
    ruleValidateOk.apply(state);       // validate correct count; no event expected
    ruleValidateFailure.apply(state);  // validate wrong count; event expected
}

// Randomized test: apply rules in a bounded random sequence to exercise
// all state transitions across the APID sequence-count lifecycle.
TEST(ApidManager, RandomizedTesting) {
    ApidManagerTester state;
    Rules::GetSeqCount::Existing ruleGetExisting;
    Rules::GetSeqCount::NewOk ruleGetNewOk;
    Rules::GetSeqCount::NewTableFull ruleGetNewTableFull;
    Rules::ValidateSeqCount::Ok ruleValidateOk;
    Rules::ValidateSeqCount::Failure ruleValidateFailure;

    STest::Rule<ApidManagerTester>* rules[] = {
        &ruleGetExisting, &ruleGetNewOk, &ruleGetNewTableFull, &ruleValidateOk, &ruleValidateFailure,
    };

    STest::RandomScenario<ApidManagerTester> random("Random Rules", rules, FW_NUM_ARRAY_ELEMENTS(rules));
    STest::BoundedScenario<ApidManagerTester> bounded("Bounded Random Rules Scenario", random, 10000);
    const U32 numSteps = bounded.run(state);
    printf("Ran %u steps.\n", numSteps);
}

}  // namespace Ccsds

}  // namespace Svc

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
