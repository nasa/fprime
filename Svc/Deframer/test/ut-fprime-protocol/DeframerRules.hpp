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
    struct GenerateFrames : public STest::Rule<Tester> {

        //! Constructor
        GenerateFrames();

        //! Precondition
        bool precondition(const Tester& state);

        //! Action
        void action(Tester& state);

    };

    //! Pack generated frames into a buffer
    //! Send the buffer
    struct SendBuffer : public STest::Rule<Tester> {

        //! Constructor
        SendBuffer();

        //! Precondition
        bool precondition(const Tester& state);

        //! Action
        void action(Tester& state);

    };

};

#endif
