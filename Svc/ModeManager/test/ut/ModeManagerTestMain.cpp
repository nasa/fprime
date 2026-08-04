// ======================================================================
// \title  ModeManagerTestMain.cpp
// \brief  cpp file for ModeManager component test main function
// ======================================================================

#include "Fw/Test/UnitTest.hpp"
#include "STest/Random/Random.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "Svc/ModeManager/test/ut/ModeManagerTester.hpp"

using Svc::ModeManagerTester;

// ----------------------------------------------------------------------
// Directed tests (default policy: checkTransition unconnected)
// ----------------------------------------------------------------------

TEST(ModeManager, Start) {
    REQUIREMENT("MM-003");
    REQUIREMENT("MM-004");
    REQUIREMENT("MM-008");
    REQUIREMENT("MM-009");
    COMMENT("START must be permitted by the default policy and drive STARTUP -> IDLE");
    ModeManagerTester tester;
    ModeManagerTester::Start__ToIdle rule;
    rule.apply(tester);
}

TEST(ModeManager, RequestModeCmd) {
    REQUIREMENT("MM-001");
    REQUIREMENT("MM-005");
    REQUIREMENT("MM-007");
    REQUIREMENT("MM-009");
    REQUIREMENT("MM-015");
    REQUIREMENT("MM-016");
    REQUIREMENT("MM-017");
    COMMENT("REQUEST_MODE must be permitted by the default policy and stamp GROUND/NONE provenance");
    ModeManagerTester tester;
    ModeManagerTester::RequestModeCmd__Transition rule;
    rule.apply(tester);
}

TEST(ModeManager, RequestModeCmdUnsupportedTarget) {
    REQUIREMENT("MM-005");
    COMMENT("Requesting STARTUP must be rejected before any policy is consulted, not asserted on");
    ModeManagerTester tester;
    ModeManagerTester::RequestModeCmd__UnsupportedTarget rule;
    rule.apply(tester);
}

TEST(ModeManager, RequestModePort) {
    REQUIREMENT("MM-001");
    REQUIREMENT("MM-005");
    REQUIREMENT("MM-007");
    REQUIREMENT("MM-010");
    REQUIREMENT("MM-011");
    REQUIREMENT("MM-012");
    REQUIREMENT("MM-015");
    REQUIREMENT("MM-016");
    REQUIREMENT("MM-017");
    COMMENT("requestMode must stamp COMPONENT provenance with requester derived from port index");
    ModeManagerTester tester;
    ModeManagerTester::RequestModePort__Transition rule;
    rule.apply(tester);
}

TEST(ModeManager, RequestModePortReservedPort) {
    REQUIREMENT("MM-021");
    COMMENT("requestMode[0] must be rejected before a ModeRequest is ever constructed");
    ModeManagerTester tester;
    ModeManagerTester::RequestModePort__ReservedPort rule;
    rule.apply(tester);
}

TEST(ModeManager, GetMode) {
    REQUIREMENT("MM-018");
    COMMENT("getMode must return the component's actual current mode");
    ModeManagerTester tester;
    ModeManagerTester::GetMode__Query rule;
    rule.apply(tester);
}

TEST(ModeManager, Ping) {
    REQUIREMENT("MM-020");
    COMMENT("pingIn must echo its key back out pingOut");
    ModeManagerTester tester;
    ModeManagerTester::Ping__Check rule;
    rule.apply(tester);
}

// ----------------------------------------------------------------------
// Randomized testing (default policy: checkTransition unconnected)
//
// Runs every mechanism rule above in random order for many steps. This
// is where sequence-dependent bugs surface: a stale "from" mode in
// ModeTransitioned, a modeChanged broadcast that skips a subscriber, or
// provenance bleeding between the ground and component request paths.
// None of these depend on the policy ever denying anything -- the
// shipped default permits everything, so what's under test is whether
// the plumbing around the decision stays correct under arbitrary
// interleaving, not whether any particular transition was "allowed".
// ----------------------------------------------------------------------

TEST(ModeManager, RandomizedTesting) {
    REQUIREMENT("MM-001");
    REQUIREMENT("MM-007");
    REQUIREMENT("MM-011");
    REQUIREMENT("MM-016");
    COMMENT("Random interleaving of every mechanism rule must never desynchronize mode, events, or notifications");

    U32 numRulesToApply = 10000;
    ModeManagerTester tester;

    ModeManagerTester::Start__ToIdle ruleStart;
    ModeManagerTester::RequestModeCmd__Transition ruleRequestCmd;
    ModeManagerTester::RequestModeCmd__UnsupportedTarget ruleRequestUnsupported;
    ModeManagerTester::RequestModePort__Transition ruleRequestPort;
    ModeManagerTester::RequestModePort__ReservedPort ruleRequestReserved;
    ModeManagerTester::GetMode__Query ruleGetMode;
    ModeManagerTester::Ping__Check rulePing;

    STest::Rule<ModeManagerTester>* rules[] = {
        &ruleStart,       &ruleRequestCmd, &ruleRequestUnsupported, &ruleRequestPort,
        &ruleRequestReserved, &ruleGetMode, &rulePing,
    };

    STest::RandomScenario<ModeManagerTester> random("Random Rules", rules, FW_NUM_ARRAY_ELEMENTS(rules));
    STest::BoundedScenario<ModeManagerTester> bounded("Bounded Random Rules Scenario", random, numRulesToApply);
    const U32 numSteps = bounded.run(tester);
    printf("Ran %u steps.\n", numSteps);
}

// ----------------------------------------------------------------------
// Directed tests (external policy: checkTransition connected)
//
// The policy stub's logic is arbitrary -- these tests only check that
// the component obeys whatever answer it gets, not that the answer was
// sensible. Deciding what SHOULD be allowed is a project's job when it
// supplies a real policy; it is out of scope for this component's own
// test suite.
// ----------------------------------------------------------------------

TEST(ModeManager, ExternalPolicyPermits) {
    REQUIREMENT("MM-005");
    REQUIREMENT("MM-006");
    COMMENT("A connected policy that returns SUCCESS must be obeyed: the transition proceeds");
    ModeManagerTester tester(true);
    ModeManagerTester::ExternalPolicy__Permits rule;
    rule.apply(tester);
}

TEST(ModeManager, ExternalPolicyDenies) {
    REQUIREMENT("MM-006");
    REQUIREMENT("MM-013");
    REQUIREMENT("MM-014");
    COMMENT("A connected policy that returns FAILURE must be obeyed: mode unchanged, TransitionRejected fired");
    ModeManagerTester tester(true);
    ModeManagerTester::ExternalPolicy__Denies rule;
    rule.apply(tester);
}

TEST(ModeManager, ExternalPolicyOverridesDefault) {
    REQUIREMENT("MM-006");
    REQUIREMENT("MM-013");
    REQUIREMENT("MM-014");
    COMMENT("An external denial must take precedence over the default, not merely be consulted alongside it");
    ModeManagerTester tester(true);
    ModeManagerTester::ExternalPolicy__DeniesEvenObviousTransition rule;
    rule.apply(tester);
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
