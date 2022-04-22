//! ======================================================================
//! DeframerRules.hpp
//! Header file for Deframer unit test rules
//! @author lestarch, bocchino
//! ======================================================================

#ifndef SVC_DEFRAMER_RULES
#define SVC_DEFRAMER_RULES

#include "Fw/Types/BasicTypes.hpp"
#include "Fw/Types/StringType.hpp"
#include "STest/STest/Pick/Pick.hpp"
#include "STest/STest/Rule/Rule.hpp"
#include "Svc/Deframer/test/ut-fprime-protocol/Tester.hpp"

namespace Svc {

    //! Generate frames to send
    struct RandomizeRule : public STest::Rule<Tester> {

        //! Constructor
        RandomizeRule();

        //! Precondition
        bool precondition(const Tester& state);

        //! Action
        void action(Tester& state);

    };

    //! Send frames
    struct SendAvailableRule : public STest::Rule<Tester> {

        //! Constructor
        SendAvailableRule();

        //! Precondition
        bool precondition(const Tester& state);

        //! Action
        void action(Tester& state);

    };

};

#endif
