/**
 * GroundInterfaceRules.hpp:
 *
 * This file specifies Rule classes for testing of the Svc::GroundInterface. These rules can then be used by the main
 * testing program to test the code. These rules support rule-based random testing.
 *
 * GroundInterface rules:
 *
 * 1. On read-callback of sufficient parts, an uplink-out is produced
 * @author lestarch
 */
#ifndef SVC_DEFRAMER_UT_UNIFIED
#define SVC_DEFRAMER_UT_UNIFIED

#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/StringType.hpp>
#include <Svc/Deframer/test/ut-unified/Tester.hpp>
#include <STest/STest/Rule/Rule.hpp>
#include <STest/STest/Pick/Pick.hpp>


namespace Svc {

    struct RandomizeRule : public STest::Rule<Tester> {
        // Constructor
        RandomizeRule(const Fw::String& name);

        // Always valid
        bool precondition(const Tester& state);

        // Will randomize the test state
        void action(Tester& truth);
    };

    struct SendAvailableRule : public STest::Rule<Tester> {
        // Constructor
        SendAvailableRule(const Fw::String& name);

        // Always valid
        bool precondition(const Tester& state);

        // Will randomize the test state
        void action(Tester& truth);
    };

};
#endif //SVC_DEFRAMER_UT_UNIFIED
