// ======================================================================
// \title  SdlsSaRouterTestMain.cpp
// \author lestarch-autobot
// \brief  cpp file for SdlsSaRouter component test main function
// ======================================================================

#include "Fw/Test/UnitTest.hpp"
#include "STest/Random/Random.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "Svc/Ccsds/SdlsSaRouter/test/ut/SdlsSaRouterTester.hpp"

using Svc::Ccsds::SdlsSaRouterTester;

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

// Verify decryptIn routes known SAs to the mapped port and passes the
// downstream status through to the caller.
TEST(SdlsSaRouter, RouteKnownSa) {
    COMMENT("Route a known SA to the mapped downstream port and pass the status through.");
    REQUIREMENT("SVC-CCSDS-SDLS-SA-ROUTER-001");
    REQUIREMENT("SVC-CCSDS-SDLS-SA-ROUTER-002");
    REQUIREMENT("SVC-CCSDS-SDLS-SA-ROUTER-003");
    SdlsSaRouterTester tester;
    SdlsSaRouterTester::Route__KnownSa rule;
    rule.apply(tester);
}

// Verify decryptIn returns UNKNOWN_SA for an unmapped SA and UNKNOWN_PORT
// for an SA mapped to an out-of-range port, without forwarding.
TEST(SdlsSaRouter, RouteErrors) {
    COMMENT("Return UNKNOWN_SA for unmapped SAs and UNKNOWN_PORT for unconnected ports.");
    REQUIREMENT("SVC-CCSDS-SDLS-SA-ROUTER-005");
    REQUIREMENT("SVC-CCSDS-SDLS-SA-ROUTER-006");
    SdlsSaRouterTester tester;
    SdlsSaRouterTester::Route__UnknownSa ruleUnknownSa;
    SdlsSaRouterTester::Route__UnknownPort ruleUnknownPort;
    ruleUnknownSa.apply(tester);
    ruleUnknownPort.apply(tester);
}

// Verify decrypted data flows upstream (saDecryptIn -> decryptOut) and its
// ownership return is routed back to the originating port
// (decryptReturnIn -> saDecryptReturnOut).
TEST(SdlsSaRouter, DecryptDataAndReturn) {
    COMMENT("Forward decrypted data upstream and route its ownership return to the originating port.");
    REQUIREMENT("SVC-CCSDS-SDLS-SA-ROUTER-004");
    SdlsSaRouterTester tester;
    SdlsSaRouterTester::DataFlow__DecryptData ruleData;
    SdlsSaRouterTester::DataFlow__DecryptReturn ruleReturn;
    ruleData.apply(tester);
    ruleReturn.apply(tester);
}

// Verify incoming iv/data buffers are passed upstream for deallocation
// (saBufferReturnIn -> bufferReturnOut).
TEST(SdlsSaRouter, BufferReturn) {
    COMMENT("Pass iv/data buffers from downstream decryptors upstream for deallocation.");
    REQUIREMENT("SVC-CCSDS-SDLS-SA-ROUTER-004");
    SdlsSaRouterTester tester;
    SdlsSaRouterTester::DataFlow__BufferReturn rule;
    rule.apply(tester);
}

// Randomized test: apply rules in a random sequence for a large number of iterations
TEST(SdlsSaRouter, RandomizedTesting) {
    COMMENT("Apply all rules in a randomized order to exercise interleaved routing and data flow.");
    REQUIREMENT("SVC-CCSDS-SDLS-SA-ROUTER-001");
    REQUIREMENT("SVC-CCSDS-SDLS-SA-ROUTER-002");
    REQUIREMENT("SVC-CCSDS-SDLS-SA-ROUTER-003");
    REQUIREMENT("SVC-CCSDS-SDLS-SA-ROUTER-004");
    REQUIREMENT("SVC-CCSDS-SDLS-SA-ROUTER-005");
    REQUIREMENT("SVC-CCSDS-SDLS-SA-ROUTER-006");
    const U32 numRulesToApply = 10000;
    SdlsSaRouterTester tester;
    SdlsSaRouterTester::Route__KnownSa ruleKnownSa;
    SdlsSaRouterTester::Route__UnknownSa ruleUnknownSa;
    SdlsSaRouterTester::Route__UnknownPort ruleUnknownPort;
    SdlsSaRouterTester::DataFlow__DecryptData ruleData;
    SdlsSaRouterTester::DataFlow__DecryptReturn ruleReturn;
    SdlsSaRouterTester::DataFlow__BufferReturn ruleBufferReturn;

    STest::Rule<SdlsSaRouterTester>* rules[] = {
        &ruleKnownSa, &ruleUnknownSa, &ruleUnknownPort, &ruleData, &ruleReturn, &ruleBufferReturn,
    };

    STest::RandomScenario<SdlsSaRouterTester> random("Random Rules", rules, FW_NUM_ARRAY_ELEMENTS(rules));
    STest::BoundedScenario<SdlsSaRouterTester> bounded("Bounded Random Rules Scenario", random, numRulesToApply);
    const U32 numSteps = bounded.run(tester);
    printf("Ran %u steps.\n", numSteps);
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
