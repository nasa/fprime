// ======================================================================
// \title  CcsdsSdlsDeframerTestMain.cpp
// \author lestarch-autobot
// \brief  cpp file for CcsdsSdlsDeframer component test main function
// ======================================================================

#include "Fw/Test/UnitTest.hpp"
#include "STest/Random/Random.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "Svc/Ccsds/CcsdsSdlsDeframer/test/ut/CcsdsSdlsDeframerTester.hpp"

using Svc::Ccsds::CcsdsSdlsDeframerTester;

// Verify dataIn extracts and removes the SA index, updates the context, and
// forwards the payload to the decryption helper.
TEST(CcsdsSdlsDeframer, DeframeNominal) {
    COMMENT("Extract and remove the SA index, update the context, and forward payload to the decryption helper.");
    REQUIREMENT("SVC-CCSDS-SDLS-DEFRAMER-001");
    REQUIREMENT("SVC-CCSDS-SDLS-DEFRAMER-002");
    REQUIREMENT("SVC-CCSDS-SDLS-DEFRAMER-003");
    CcsdsSdlsDeframerTester tester;
    CcsdsSdlsDeframerTester::Deframe__Nominal rule;
    rule.apply(tester);
}

// Verify a too-short frame raises InsufficientLength and is returned.
TEST(CcsdsSdlsDeframer, DeframeInsufficientLength) {
    COMMENT("Raise InsufficientLength and return the buffer when the frame cannot hold an SA index.");
    REQUIREMENT("SVC-CCSDS-SDLS-DEFRAMER-004");
    CcsdsSdlsDeframerTester tester;
    CcsdsSdlsDeframerTester::Deframe__InsufficientLength rule;
    rule.apply(tester);
}

// Verify a decryption failure raises DecryptionFailed, notifies errorNotify,
// and returns the frame buffer.
TEST(CcsdsSdlsDeframer, DeframeDecryptFailure) {
    COMMENT("Raise DecryptionFailed, notify errorNotify, and return the buffer on a bad decrypt status.");
    REQUIREMENT("SVC-CCSDS-SDLS-DEFRAMER-005");
    CcsdsSdlsDeframerTester tester;
    CcsdsSdlsDeframerTester::Deframe__DecryptFailure rule;
    rule.apply(tester);
}

// Verify decrypted data passes downstream and ownership returns are routed.
TEST(CcsdsSdlsDeframer, DataFlowPaths) {
    COMMENT("Pass decrypted data downstream and route ownership returns in both directions.");
    REQUIREMENT("SVC-CCSDS-SDLS-DEFRAMER-006");
    REQUIREMENT("SVC-CCSDS-SDLS-DEFRAMER-007");
    REQUIREMENT("SVC-CCSDS-SDLS-DEFRAMER-008");
    CcsdsSdlsDeframerTester tester;
    CcsdsSdlsDeframerTester::DataFlow__DecryptedData ruleData;
    CcsdsSdlsDeframerTester::DataFlow__DataReturn ruleReturn;
    CcsdsSdlsDeframerTester::DataFlow__BufferReturn ruleBufferReturn;
    ruleData.apply(tester);
    ruleReturn.apply(tester);
    ruleBufferReturn.apply(tester);
}

// Randomized test: apply rules in a random sequence for a large number of iterations
TEST(CcsdsSdlsDeframer, RandomizedTesting) {
    COMMENT("Apply all rules in a randomized order to exercise interleaved deframing and data flow.");
    REQUIREMENT("SVC-CCSDS-SDLS-DEFRAMER-001");
    REQUIREMENT("SVC-CCSDS-SDLS-DEFRAMER-002");
    REQUIREMENT("SVC-CCSDS-SDLS-DEFRAMER-003");
    REQUIREMENT("SVC-CCSDS-SDLS-DEFRAMER-004");
    REQUIREMENT("SVC-CCSDS-SDLS-DEFRAMER-005");
    REQUIREMENT("SVC-CCSDS-SDLS-DEFRAMER-006");
    REQUIREMENT("SVC-CCSDS-SDLS-DEFRAMER-007");
    REQUIREMENT("SVC-CCSDS-SDLS-DEFRAMER-008");
    const U32 numRulesToApply = 10000;
    CcsdsSdlsDeframerTester tester;
    CcsdsSdlsDeframerTester::Deframe__Nominal ruleNominal;
    CcsdsSdlsDeframerTester::Deframe__InsufficientLength ruleInsufficient;
    CcsdsSdlsDeframerTester::Deframe__DecryptFailure ruleFailure;
    CcsdsSdlsDeframerTester::DataFlow__DecryptedData ruleData;
    CcsdsSdlsDeframerTester::DataFlow__DataReturn ruleReturn;
    CcsdsSdlsDeframerTester::DataFlow__BufferReturn ruleBufferReturn;

    STest::Rule<CcsdsSdlsDeframerTester>* rules[] = {
        &ruleNominal, &ruleInsufficient, &ruleFailure, &ruleData, &ruleReturn, &ruleBufferReturn,
    };
    STest::RandomScenario<CcsdsSdlsDeframerTester> randomScenario("RandomScenario", rules,
                                                                  FW_NUM_ARRAY_ELEMENTS(rules));
    STest::BoundedScenario<CcsdsSdlsDeframerTester> boundedScenario("BoundedScenario", randomScenario, numRulesToApply);
    const U32 numSteps = boundedScenario.run(tester);
    ASSERT_EQ(numSteps, numRulesToApply);
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
